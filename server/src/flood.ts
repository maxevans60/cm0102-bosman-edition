// Layer 1 against call floods (0.7.0, Max's decision of 2026-10-29: stay on the free plan).
//
// 1. A limit for each internet address, checked first in the Worker, before any body is read and
//    before any Durable Object is asked. It uses the Workers Rate Limiting binding (wrangler.jsonc:
//    RATE_CALLS for every call, RATE_WRITES also for register, join and close). Its counts are
//    approximate and kept for each Cloudflare location, which is enough against one address.
// 2. A copy of the list and of one game's answer in the Worker's memory for ANSWER_CACHE_MS, so
//    many list calls cost one lobby call. The answers hold no internet address and no token.
//
// The limiter never gets an internet address (Q26): its key is a hash of the address's prefix and
// the UTC date, so the same address has a new key each day. Nothing here is stored or logged.
// A refused call still counts as a Worker request: Cloudflare counts each call that reaches the
// Worker. The limit saves the Durable Object requests, storage writes and run time behind it.
import { sha256 } from "./crypto";
import type { Result } from "./http";
import { addressPrefix } from "./lobby";

/** Calls in 60 s from one address. Must match RATE_CALLS in wrangler.jsonc. */
export const CALL_LIMIT = 120;
/** Registers, joins and closes in 60 s from one address. Must match RATE_WRITES in wrangler.jsonc. */
export const WRITE_LIMIT = 10;
export const REFUSED_RETRY_SECONDS = 60;
export const ANSWER_CACHE_MS = 2_000;
let cacheMs = ANSWER_CACHE_MS;
const ANSWER_CACHE_MAX = 1_000;

interface Limiter {
  limit(options: { key: string }): Promise<{ success: boolean }>;
}

export async function limiterKey(address: string, now = Date.now()): Promise<string> {
  const day = new Date(now).toISOString().slice(0, 10);
  const h = await sha256(`bosman-rate|${day}|${addressPrefix(address)}`);
  return [...h.slice(0, 16)].map((b) => b.toString(16).padStart(2, "0")).join("");
}

/** True when the call may go on. A missing or failing limiter lets the call through. */
async function allow(limiter: Limiter | undefined, key: string): Promise<boolean> {
  if (limiter === undefined) return true;
  try {
    return (await limiter.limit({ key })).success;
  } catch {
    return true;
  }
}

/** Register, join and close: the calls that write storage, hash a password or make a relay. */
export function isWrite(method: string, parts: string[]): boolean {
  if (parts[0] !== "v1" || parts[1] !== "games") return false;
  if (parts.length === 2) return method === "POST";
  if (parts.length === 3) return method === "DELETE";
  return parts.length === 4 && parts[3] === "join" && method === "POST";
}

/** Checks the limits for this call. Calls with no CF-Connecting-IP (only local runs) are not limited. */
export async function withinLimits(request: Request, env: Env, parts: string[]): Promise<boolean> {
  const address = request.headers.get("CF-Connecting-IP");
  if (address === null) return true;
  const limits = env as unknown as { RATE_CALLS?: Limiter; RATE_WRITES?: Limiter };
  const key = await limiterKey(address);
  if (!(await allow(limits.RATE_CALLS, key))) return false;
  return !isWrite(request.method, parts) || (await allow(limits.RATE_WRITES, key));
}

const REFUSED_JSON = JSON.stringify({
  error: "unavailable",
  message: "Online games are not available now, try again later",
  retryAfterSeconds: REFUSED_RETRY_SECONDS,
});
const REFUSED_PAGE =
  '<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">' +
  "<title>Try again in a minute</title><p>Too many requests from your network. Try again in a minute.</p>";

/** The game's "not available" answer (lobby_http.c: LH_UNAVAILABLE), or a short page for the invite link. */
export function refused(page: boolean): Response {
  const common = { "Cache-Control": "no-store", "Retry-After": String(REFUSED_RETRY_SECONDS) };
  return page
    ? new Response(REFUSED_PAGE, { status: 503, headers: { ...common, "Content-Type": "text/html; charset=utf-8" } })
    : new Response(REFUSED_JSON, {
        status: 503,
        headers: { ...common, "Content-Type": "application/json; charset=utf-8", "X-Bosman-Api": "1" },
      });
}

const answers = new Map<string, { until: number; result: Result }>();

/** Gives the lobby's answer from memory when it is less than ANSWER_CACHE_MS old. Errors are not kept. */
export async function cachedAnswer(name: string, ask: () => Promise<Result>): Promise<Result> {
  const now = Date.now();
  const hit = answers.get(name);
  if (cacheMs > 0 && hit !== undefined && now < hit.until) return hit.result;
  const result = await ask();
  if (result.ok || result.error === "unknown_game") {
    if (answers.size >= ANSWER_CACHE_MAX) answers.clear();
    answers.set(name, { until: now + cacheMs, result });
  }
  return result;
}

export function clearAnswerCache() {
  answers.clear();
}

/** Test hook: how long answers stay in memory. 0 turns the copy off. */
export function setAnswerCacheMs(ms: number) {
  cacheMs = ms;
  answers.clear();
}

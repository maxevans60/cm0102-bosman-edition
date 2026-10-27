// The lobby: one Durable Object that keeps the list of open online games.
// Everything lives in memory only, so no stored record holds an internet address (Q26).
// If the object restarts, a heartbeat answers unknown_game and the host registers again
// with its code and host token, so the game keeps its code and its relay.
import { DurableObject } from "cloudflare:workers";
import { hashPassword, randomBytes, randomCode, sameBytes, sha256 } from "./crypto";
import { err, type Result } from "./http";
import type { HeartbeatInput, JoinInput, PublicInfo, RegisterInput } from "./validate";

export const HEARTBEAT_SECONDS = 30;
export const EXPIRE_SECONDS = 90;
export const TICKET_SECONDS = 60;
export const MAX_GAMES_PER_ADDRESS = 2;
export const MAX_LISTED = 200;
export const MAX_OPEN_GAMES = 300;
/** Wrong passwords allowed in one minute from one address, and from all addresses together. */
export const MAX_FAILED_PER_ADDRESS = 5;
export const MAX_FAILED_PER_GAME = 30;
const FAILED_PASSWORD_WINDOW_MS = 60_000;

interface Failures {
  since: number;
  count: number;
}

interface Game {
  code: string;
  gameId: string;
  /** A salted hash of the host's internet address. The salt is random and lives in memory only. */
  addressKey: string;
  info: PublicInfo;
  hostTokenHash: Uint8Array;
  password: { salt: Uint8Array; hash: Uint8Array } | null;
  lastSeen: number;
  /** Wrong passwords in the current minute: in total, and for each salted address hash. */
  failed: Failures;
  failedBy: Map<string, Failures>;
}

/**
 * The part of an internet address that names one home connection. An IPv6 home connection
 * gets a whole /64, so the lobby counts IPv6 addresses by their first 64 bits.
 */
export function addressPrefix(address: string): string {
  if (!address.includes(":")) return address;
  const [head, tail = ""] = address.toLowerCase().split("::");
  const h = head === "" ? [] : head.split(":");
  const t = tail === "" ? [] : tail.split(":");
  const groups = address.includes("::") ? [...h, ...Array(Math.max(0, 8 - h.length - t.length)).fill("0"), ...t] : h;
  return groups
    .slice(0, 4)
    .map((g) => g.replace(/^0+(?=.)/, ""))
    .join(":");
}

function hex(b: Uint8Array): string {
  return [...b].map((x) => x.toString(16).padStart(2, "0")).join("");
}

export class Lobby extends DurableObject<Env> {
  /** The clock. Tests replace it. */
  now: () => number = () => Date.now();

  private games = new Map<string, Game>();
  private byGameId = new Map<string, string>();
  private readonly addressSalt = randomBytes(32);

  private relay(code: string) {
    return this.env.RELAY.get(this.env.RELAY.idFromName(code));
  }

  private async addressKey(address: string): Promise<string> {
    const a = new TextEncoder().encode(addressPrefix(address));
    const buf = new Uint8Array(this.addressSalt.length + a.length);
    buf.set(this.addressSalt);
    buf.set(a, this.addressSalt.length);
    return hex(await sha256(buf));
  }

  private gamesAt(addressKey: string, except?: string): number {
    let n = 0;
    for (const g of this.games.values()) if (g.addressKey === addressKey && g.code !== except) n++;
    return n;
  }

  private remove(g: Game) {
    this.games.delete(g.code);
    if (this.byGameId.get(g.gameId) === g.code) this.byGameId.delete(g.gameId);
  }

  private async teardown(code: string) {
    try {
      await this.relay(code).teardown();
    } catch {
      // The relay also closes when its host leaves. Nothing more to do here.
    }
  }

  /** Drops every game with no heartbeat for EXPIRE_SECONDS, and closes its relay. */
  private async prune() {
    const limit = this.now() - EXPIRE_SECONDS * 1000;
    const dropped: string[] = [];
    for (const g of this.games.values()) {
      if (g.lastSeen <= limit) {
        this.remove(g);
        dropped.push(g.code);
      }
    }
    await Promise.all(dropped.map((c) => this.teardown(c)));
  }

  private async isHost(g: Game, token: string | null): Promise<boolean> {
    return token !== null && sameBytes(await sha256(token), g.hostTokenHash);
  }

  private view(g: Game) {
    return { code: g.code, ...g.info, password: g.password !== null };
  }

  private async passwordRecord(password: string | null) {
    if (password === null) return null;
    const salt = randomBytes(16);
    return { salt, hash: await hashPassword(password, salt) };
  }

  private registered(g: Game, token: string, status: number): Result {
    return {
      ok: true,
      status,
      body: { code: g.code, hostToken: token, heartbeatSeconds: HEARTBEAT_SECONDS, expireSeconds: EXPIRE_SECONDS },
    };
  }

  async register(input: RegisterInput, address: string): Promise<Result> {
    await this.prune();
    const addressKey = await this.addressKey(address);
    const { gameId, code: oldCode, hostToken: oldToken, password: plain, ...info } = input;

    if (oldCode !== null || oldToken !== null) {
      if (oldCode === null || oldToken === null) return err("bad_request", { field: oldCode === null ? "code" : "hostToken" });
      return this.restore(oldCode, oldToken, gameId, info, plain, addressKey);
    }

    // The cheap checks come first, and again after each wait: other requests can run meanwhile.
    const check = (): Result | null => {
      const open = this.byGameId.get(gameId);
      if (open !== undefined) return err("game_already_open", { code: open });
      if (this.gamesAt(addressKey) >= MAX_GAMES_PER_ADDRESS) return err("too_many_games");
      if (this.games.size >= MAX_OPEN_GAMES) return err("lobby_full");
      return null;
    };
    for (let attempt = 0; attempt < 5; attempt++) {
      const refused = check();
      if (refused !== null) return refused;
      const code = randomCode();
      if (this.games.has(code)) continue;
      const token = await this.relay(code).setup(code);
      if (token === null) continue; // the code still has a live relay
      const tokenHash = await sha256(token);
      const password = await this.passwordRecord(plain);
      const raced = check();
      if (raced !== null || this.games.has(code)) {
        await this.teardown(code);
        if (raced !== null) return raced;
        continue;
      }
      const g: Game = {
        code,
        gameId,
        addressKey,
        info,
        hostTokenHash: tokenHash,
        password,
        lastSeen: this.now(),
        failed: { since: 0, count: 0 },
        failedBy: new Map(),
      };
      this.games.set(code, g);
      this.byGameId.set(gameId, code);
      return this.registered(g, token, 201);
    }
    return err("unavailable");
  }

  /** A host registers its game again with its code and token, after a lost heartbeat or a lobby restart. */
  private async restore(
    code: string,
    token: string,
    gameId: string,
    info: PublicInfo,
    plain: string | null,
    addressKey: string,
  ): Promise<Result> {
    const tokenHash = await sha256(token);
    if (!this.games.has(code) && !(await this.relay(code).verifyHost(token))) return err("not_host");
    const password = await this.passwordRecord(plain);
    // From here on nothing waits, so no other request can change the game in between.
    const g = this.games.get(code);
    if (g !== undefined && !sameBytes(tokenHash, g.hostTokenHash)) return err("not_host");
    const open = this.byGameId.get(gameId);
    if (open !== undefined && open !== code) return err("game_already_open", { code: open });
    if (this.gamesAt(addressKey, code) >= MAX_GAMES_PER_ADDRESS) return err("too_many_games");
    if (g === undefined && this.games.size >= MAX_OPEN_GAMES) return err("lobby_full");
    if (g !== undefined && g.gameId !== gameId && this.byGameId.get(g.gameId) === code) this.byGameId.delete(g.gameId);
    const restored: Game = {
      code,
      gameId,
      addressKey,
      info,
      hostTokenHash: tokenHash,
      password,
      lastSeen: this.now(),
      failed: g?.failed ?? { since: 0, count: 0 },
      failedBy: g?.failedBy ?? new Map(),
    };
    this.games.set(code, restored);
    this.byGameId.set(gameId, code);
    return this.registered(restored, token, 200);
  }

  async heartbeat(code: string, token: string | null, input: HeartbeatInput): Promise<Result> {
    await this.prune();
    const g = this.games.get(code);
    if (g === undefined) return err("unknown_game");
    if (!(await this.isHost(g, token))) return err("not_host");
    Object.assign(g.info, input);
    g.lastSeen = this.now();
    return { ok: true, body: { ok: true } };
  }

  async close(code: string, token: string | null): Promise<Result> {
    await this.prune();
    const g = this.games.get(code);
    if (g === undefined) return err("unknown_game");
    if (!(await this.isHost(g, token))) return err("not_host");
    this.remove(g);
    await this.teardown(code);
    return { ok: true, body: { ok: true } };
  }

  async list(): Promise<Result> {
    await this.prune();
    const games = [...this.games.values()]
      .sort((a, b) => b.info.managersOnline - a.info.managersOnline || a.info.name.localeCompare(b.info.name))
      .slice(0, MAX_LISTED)
      .map((g) => this.view(g));
    return { ok: true, body: { games, heartbeatSeconds: HEARTBEAT_SECONDS } };
  }

  async get(code: string): Promise<Result> {
    await this.prune();
    const g = this.games.get(code);
    return g === undefined ? err("unknown_game") : { ok: true, body: { game: this.view(g) } };
  }

  async isOpen(code: string): Promise<boolean> {
    await this.prune();
    return this.games.has(code);
  }

  async join(code: string, input: JoinInput, address: string): Promise<Result> {
    await this.prune();
    const g = this.games.get(code);
    if (g === undefined) return err("unknown_game");
    if (input.version !== g.info.version) return err("version_mismatch", { hostVersion: g.info.version });
    if (g.password !== null) {
      const addressKey = await this.addressKey(address);
      const now = this.now();
      const fresh = (f: Failures | undefined): Failures =>
        f !== undefined && now - f.since < FAILED_PASSWORD_WINDOW_MS ? f : { since: now, count: 0 };
      for (const [k, f] of g.failedBy) if (fresh(f) !== f) g.failedBy.delete(k);
      g.failed = fresh(g.failed);
      const mine = fresh(g.failedBy.get(addressKey));
      g.failedBy.set(addressKey, mine);
      if (mine.count >= MAX_FAILED_PER_ADDRESS || g.failed.count >= MAX_FAILED_PER_GAME) return err("too_many_attempts");
      // Count the try before the hash, so tries sent at the same time cannot pass the limit.
      mine.count++;
      g.failed.count++;
      const ok = input.password !== null && sameBytes(await hashPassword(input.password, g.password.salt), g.password.hash);
      if (!ok) return err("wrong_password");
      mine.count--;
      g.failed.count--;
    }
    const ticket = await this.relay(code).mintTicket();
    if (ticket === null) return err("unknown_game");
    return { ok: true, body: { ticket, expireSeconds: TICKET_SECONDS } };
  }

  // Test hooks. They read memory only and return no internet address.

  /** Forgets every game, as a restart does. */
  forgetAll() {
    this.games.clear();
    this.byGameId.clear();
  }

  addressCount(): number {
    return new Set([...this.games.values()].map((g) => g.addressKey)).size;
  }

  debugDump(): string {
    const dec = (b: Uint8Array) => new TextDecoder("latin1").decode(b);
    return JSON.stringify([...this.games.values()], (_k, v) => (v instanceof Uint8Array ? dec(v) + hex(v) : v));
  }
}

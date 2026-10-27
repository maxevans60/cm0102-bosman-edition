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
const MAX_FAILED_PASSWORDS = 5;
const FAILED_PASSWORD_WINDOW_MS = 60_000;

interface Game {
  code: string;
  gameId: string;
  /** A salted hash of the host's internet address. The salt is random and lives in memory only. */
  addressKey: string;
  info: PublicInfo;
  hostTokenHash: Uint8Array;
  password: { salt: Uint8Array; hash: Uint8Array } | null;
  lastSeen: number;
  failed: { since: number; count: number };
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
    const a = new TextEncoder().encode(address);
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
    const password = await this.passwordRecord(input.password);
    const { gameId, code: oldCode, hostToken: oldToken, password: _pw, ...info } = input;

    if (oldCode !== null || oldToken !== null) {
      if (oldCode === null || oldToken === null) return err("bad_request", { field: oldCode === null ? "code" : "hostToken" });
      return this.restore(oldCode, oldToken, gameId, info, password, addressKey);
    }

    for (let attempt = 0; attempt < 5; attempt++) {
      const open = this.byGameId.get(gameId);
      if (open !== undefined) return err("game_already_open", { code: open });
      if (this.gamesAt(addressKey) >= MAX_GAMES_PER_ADDRESS) return err("too_many_games");
      const code = randomCode();
      if (this.games.has(code)) continue;
      const token = await this.relay(code).setup();
      if (token === null) continue; // the code still has a live relay
      const tokenHash = await sha256(token);
      // Check again: other requests can run while this one waits for the relay.
      const raced = this.byGameId.get(gameId);
      const full = this.gamesAt(addressKey) >= MAX_GAMES_PER_ADDRESS;
      if (raced !== undefined || full || this.games.has(code)) {
        await this.teardown(code);
        if (raced !== undefined) return err("game_already_open", { code: raced });
        if (full) return err("too_many_games");
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
    password: Game["password"],
    addressKey: string,
  ): Promise<Result> {
    const tokenHash = await sha256(token);
    if (!this.games.has(code) && !(await this.relay(code).verifyHost(token))) return err("not_host");
    // From here on nothing waits, so no other request can change the game in between.
    const g = this.games.get(code);
    if (g !== undefined && !sameBytes(tokenHash, g.hostTokenHash)) return err("not_host");
    const open = this.byGameId.get(gameId);
    if (open !== undefined && open !== code) return err("game_already_open", { code: open });
    if (this.gamesAt(addressKey, code) >= MAX_GAMES_PER_ADDRESS) return err("too_many_games");
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

  async join(code: string, input: JoinInput): Promise<Result> {
    await this.prune();
    const g = this.games.get(code);
    if (g === undefined) return err("unknown_game");
    if (input.version !== g.info.version) return err("version_mismatch", { hostVersion: g.info.version });
    if (g.password !== null) {
      const now = this.now();
      if (now - g.failed.since >= FAILED_PASSWORD_WINDOW_MS) g.failed = { since: now, count: 0 };
      if (g.failed.count >= MAX_FAILED_PASSWORDS) return err("too_many_attempts");
      const ok = input.password !== null && sameBytes(await hashPassword(input.password, g.password.salt), g.password.hash);
      if (!ok) {
        g.failed.count++;
        return err("wrong_password");
      }
    }
    const ticket = await this.relay(code).mintTicket();
    if (ticket === null) return err("unknown_game");
    return { ok: true, body: { ticket, expireSeconds: TICKET_SECONDS } };
  }

  // Test hooks. They read memory only and return no internet address.

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

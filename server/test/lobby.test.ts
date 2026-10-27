import { abortAllDurableObjects } from "cloudflare:test";
import { afterEach, beforeEach, describe, expect, it } from "vitest";
import { call, freshIp, gameBody, json, register, setLobbyClock, withLobby } from "./helpers";

beforeEach(async () => {
  await abortAllDurableObjects();
});
afterEach(async () => {
  await abortAllDurableObjects();
});

describe("register", () => {
  it("returns a game code, a host token and the heartbeat times", async () => {
    const { res, data } = await register();
    expect(res.status).toBe(201);
    expect(data.code).toMatch(/^[23456789ABCDEFGHJKMNPQRSTUVWXYZ]{8}$/);
    expect(typeof data.hostToken).toBe("string");
    expect(data.heartbeatSeconds).toBe(30);
    expect(data.expireSeconds).toBe(90);
  });

  it("puts the game in the list with every public field and no private field", async () => {
    const { code } = await register({ password: "secret" });
    const data = await json(await call("/v1/games"));
    const game = data.games.find((g: any) => g.code === code);
    expect(game).toEqual({
      code,
      name: "Sunday League",
      hostName: "Max",
      version: "0.7.0",
      database: "Original 2001/02",
      season: 2001,
      day: "2001-08-11",
      humanClubs: ["Arsenal", "Scunthorpe United"],
      freeSeats: 14,
      managersOnline: 2,
      password: true,
    });
    expect(JSON.stringify(data)).not.toContain("secret");
    expect(JSON.stringify(data)).not.toContain("gameId");
  });

  it("shows one game by its code", async () => {
    const { code } = await register();
    const res = await call(`/v1/games/${code}`);
    expect(res.status).toBe(200);
    expect((await json(res)).game.code).toBe(code);
    const missing = await call(`/v1/games/ZZZZZZZZ`);
    expect(missing.status).toBe(404);
    expect((await json(missing)).error).toBe("unknown_game");
  });

  it.each([
    ["gameId", ""],
    ["gameId", "x".repeat(200)],
    ["name", ""],
    ["name", "   "],
    ["name", "x".repeat(41)],
    ["name", "tab\there"],
    ["name", "emoji \u{1F600}"],
    ["hostName", 7],
    ["version", ""],
    ["season", 1800],
    ["season", "2001"],
    ["day", "11/08/2001"],
    ["humanClubs", "Arsenal"],
    ["humanClubs", Array(17).fill("Club")],
    ["freeSeats", -1],
    ["freeSeats", 17],
    ["managersOnline", 1.5],
    ["password", "x".repeat(65)],
  ])("rejects a bad %s", async (field, value) => {
    const res = await call("/v1/games", { method: "POST", body: gameBody({ [field]: value }), ip: freshIp() });
    expect(res.status).toBe(400);
    expect(await json(res)).toMatchObject({ error: "bad_request", field });
  });

  it("accepts names with Latin-1 letters", async () => {
    const { res } = await register({ name: "Liga Española", hostName: "Zoë" });
    expect(res.status).toBe(201);
  });
});

describe("word filter", () => {
  it.each(["name", "hostName"])("rejects a rude %s", async (field) => {
    const res = await call("/v1/games", { method: "POST", body: gameBody({ [field]: "F.U.C.K league" }), ip: freshIp() });
    expect(res.status).toBe(400);
    expect(await json(res)).toEqual(expect.objectContaining({ error: "name_not_allowed", field }));
  });

  it("rejects a rude manager name at the join step", async () => {
    const { code } = await register();
    const res = await call(`/v1/games/${code}/join`, {
      method: "POST",
      ip: freshIp(),
      body: { version: "0.7.0", name: "sh1t" },
    });
    expect(res.status).toBe(400);
    expect(await json(res)).toMatchObject({ error: "name_not_allowed", field: "name" });
  });
});

describe("heartbeat and expiry", () => {
  it("drops a game with no heartbeat for 90 seconds", async () => {
    const t0 = 1_800_000_000_000;
    await setLobbyClock(t0);
    const { code } = await register();
    await setLobbyClock(t0 + 89_999);
    expect((await json(await call("/v1/games"))).games.map((g: any) => g.code)).toContain(code);
    await setLobbyClock(t0 + 90_000);
    expect((await json(await call("/v1/games"))).games.map((g: any) => g.code)).not.toContain(code);
  });

  it("keeps a game that sends a heartbeat every 30 seconds, and updates its fields", async () => {
    const t0 = 1_800_000_000_000;
    await setLobbyClock(t0);
    const { code, hostToken } = await register();
    for (let i = 1; i <= 6; i++) {
      await setLobbyClock(t0 + i * 30_000);
      const res = await call(`/v1/games/${code}/heartbeat`, {
        method: "POST",
        token: hostToken,
        body: { day: "2001-08-18", managersOnline: 3, freeSeats: 13, season: 2001, humanClubs: ["Arsenal"] },
      });
      expect(res.status).toBe(200);
      expect(await json(res)).toEqual({ ok: true });
    }
    const game = (await json(await call(`/v1/games/${code}`))).game;
    expect(game).toMatchObject({ day: "2001-08-18", managersOnline: 3, freeSeats: 13, humanClubs: ["Arsenal"] });
  });

  it("accepts a heartbeat with no body", async () => {
    const { code, hostToken } = await register();
    const res = await call(`/v1/games/${code}/heartbeat`, { method: "POST", token: hostToken });
    expect(res.status).toBe(200);
  });

  it("answers unknown_game to a heartbeat after expiry, so the host registers again", async () => {
    const t0 = 1_800_000_000_000;
    await setLobbyClock(t0);
    const { code, hostToken } = await register();
    await setLobbyClock(t0 + 91_000);
    const res = await call(`/v1/games/${code}/heartbeat`, { method: "POST", token: hostToken });
    expect(res.status).toBe(404);
    expect((await json(res)).error).toBe("unknown_game");
  });

  it("refuses a heartbeat and a close without the host token", async () => {
    const { code } = await register();
    for (const token of [undefined, "wrong"]) {
      const hb = await call(`/v1/games/${code}/heartbeat`, { method: "POST", token });
      expect(hb.status).toBe(401);
      expect((await json(hb)).error).toBe("not_host");
      const del = await call(`/v1/games/${code}`, { method: "DELETE", token });
      expect(del.status).toBe(401);
    }
  });

  it("rejects a bad field in a heartbeat", async () => {
    const { code, hostToken } = await register();
    const res = await call(`/v1/games/${code}/heartbeat`, { method: "POST", token: hostToken, body: { freeSeats: 99 } });
    expect(res.status).toBe(400);
    expect((await json(res)).field).toBe("freeSeats");
  });
});

describe("close", () => {
  it("removes the game from the list", async () => {
    const { code, hostToken } = await register();
    const res = await call(`/v1/games/${code}`, { method: "DELETE", token: hostToken });
    expect(res.status).toBe(200);
    expect((await json(await call("/v1/games"))).games.map((g: any) => g.code)).not.toContain(code);
    expect((await call(`/v1/games/${code}`)).status).toBe(404);
  });
});

describe("at most 2 open games for each internet address", () => {
  it("refuses a third game from one address and allows it after a close", async () => {
    const ip = freshIp();
    const a = await register({}, ip);
    const b = await register({}, ip);
    expect(a.res.status).toBe(201);
    expect(b.res.status).toBe(201);
    const c = await register({}, ip);
    expect(c.res.status).toBe(429);
    expect(c.data.error).toBe("too_many_games");
    expect((await register({}, freshIp())).res.status).toBe(201);
    await call(`/v1/games/${a.code}`, { method: "DELETE", token: a.hostToken });
    expect((await register({}, ip)).res.status).toBe(201);
  });

  it("frees the address when a game expires", async () => {
    const t0 = 1_800_000_000_000;
    await setLobbyClock(t0);
    const ip = freshIp();
    await register({}, ip);
    await register({}, ip);
    await setLobbyClock(t0 + 90_000);
    expect((await register({}, ip)).res.status).toBe(201);
  });
});

describe("one open copy of a game", () => {
  it("refuses a second host of the same save and names the open copy", async () => {
    const first = await register();
    const again = await register({ gameId: first.body.gameId });
    expect(again.res.status).toBe(409);
    expect(again.data).toMatchObject({ error: "game_already_open", code: first.code });
    expect(again.data.hostToken).toBeUndefined();
  });

  it("allows the save again after the first copy closes", async () => {
    const first = await register();
    await call(`/v1/games/${first.code}`, { method: "DELETE", token: first.hostToken });
    expect((await register({ gameId: first.body.gameId })).res.status).toBe(201);
  });

  it("lets the host register its own game again with its code and token", async () => {
    const first = await register();
    const again = await register({ gameId: first.body.gameId, code: first.code, hostToken: first.hostToken });
    expect(again.res.status).toBe(200);
    expect(again.data).toMatchObject({ code: first.code, hostToken: first.hostToken });
  });

  it("restores a game with the same code after the lobby forgets it", async () => {
    const first = await register();
    await withLobby((inst) => inst.forgetAll());
    const again = await register({ gameId: first.body.gameId, code: first.code, hostToken: first.hostToken });
    expect(again.res.status).toBe(200);
    expect(again.data.code).toBe(first.code);
    const wrong = await register({ code: first.code, hostToken: "nope" });
    expect(wrong.res.status).toBe(401);
  });
});

describe("join step", () => {
  it("compares the version and names the host's version", async () => {
    const { code } = await register({ version: "0.7.0" });
    const res = await call(`/v1/games/${code}/join`, { method: "POST", ip: freshIp(), body: { version: "0.6.0", name: "Ann" } });
    expect(res.status).toBe(409);
    expect(await json(res)).toMatchObject({ error: "version_mismatch", hostVersion: "0.7.0" });
  });

  it("checks the password on the server", async () => {
    const { code } = await register({ password: "open sesame" });
    const none = await call(`/v1/games/${code}/join`, { method: "POST", ip: freshIp(), body: { version: "0.7.0", name: "Ann" } });
    expect(none.status).toBe(403);
    expect((await json(none)).error).toBe("wrong_password");
    const bad = await call(`/v1/games/${code}/join`, {
      method: "POST",
      ip: freshIp(),
      body: { version: "0.7.0", name: "Ann", password: "open sesamE" },
    });
    expect(bad.status).toBe(403);
    const good = await call(`/v1/games/${code}/join`, {
      method: "POST",
      ip: freshIp(),
      body: { version: "0.7.0", name: "Ann", password: "open sesame" },
    });
    expect(good.status).toBe(200);
    const data = await json(good);
    expect(typeof data.ticket).toBe("string");
    expect(data.expireSeconds).toBe(60);
  });

  it("does not keep the password in plain text", async () => {
    await register({ password: "plain-text-check" });
    const dump = await withLobby((inst) => inst.debugDump());
    expect(dump).not.toContain("plain-text-check");
  });

  it("slows down password guessing", async () => {
    const { code } = await register({ password: "right" });
    let last: Response | null = null;
    for (let i = 0; i < 6; i++) {
      last = await call(`/v1/games/${code}/join`, {
        method: "POST",
        ip: freshIp(),
        body: { version: "0.7.0", name: "Ann", password: `guess${i}` },
      });
    }
    expect(last!.status).toBe(429);
    expect((await json(last!)).error).toBe("too_many_attempts");
  });

  it("answers unknown_game for a game that is not open", async () => {
    const res = await call(`/v1/games/ABCDEFGH/join`, { method: "POST", ip: freshIp(), body: { version: "0.7.0", name: "Ann" } });
    expect(res.status).toBe(404);
    expect((await json(res)).error).toBe("unknown_game");
  });
});

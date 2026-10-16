# Research notes: easy online play (#45, 0.7.0)

Status: first notes from a talk with Max on 2026-10-16. No code research yet. The open decisions go to the grilling skill before a spec.

## 1. What the game does today

- The original has a network game: one player hosts the game, the others connect to the host. It uses WinSock TCP.
- The port answers every network call with "no network" (`src/recomp_rt/win/net.c`).
- Players who want a network game today need an extra patch and a VPN tool such as Tailscale, so that their computers can find each other.

## 2. The problem

Home routers block incoming connections. A friend cannot type the host's address and join. The fix must work without router settings and without an IP address.

## 3. Ways to connect

1. **Join code and a small helper server (recommended).** The host clicks "Host online game" and gets a short code, for example `TIGER-42`. The friend types the code. The helper server gives each computer the address of the other and helps them connect direct (hole punching). If a router still blocks the connection, the server passes the traffic between them (relay). The server sees the connection only, not the saved game.
2. **A VPN library inside the game** (for example ZeroTier's libzt). It works as Tailscale does, with nothing to install. No server of ours, but it depends on the service of another company.
3. **Local network only.** It works at home or over a VPN that the players already use, with no server. It does not solve play over the internet.

Every way needs the same base: real sockets behind the game's WinSock calls in `net.c`.

## 4. Helper server cost (estimates, prices not checked on 2026-10-16)

| Option | Cost a month | Notes |
|---|---|---|
| Cloudflare Worker (Max has an account) | £0 on the free plan for light use; about $5 on the paid plan if each game needs a Durable Object | Join codes, and relay over WebSockets. Recommended. |
| A small rented server (Hetzner, DigitalOcean) | about £4 to £6 | Join codes and relay over TCP or UDP. |
| A free tier (for example Oracle Cloud always-free) | £0 | Less support. The free offer can change. |

The traffic is expected to be small: the game exchanges data once for each game day, not in real time. The real amount is unknown until the network protocol is researched. Most players connect direct after the join code, so the relay carries traffic only for players whose routers block that.

## 5. Other points

- Lag matters little: the game waits for every player before a day moves on.
- Both players must have the same version and the same database, or the games drift apart. The join step should check this.
- Unknown: whether the original's network protocol has surprises (timeouts, message sizes, what the host sends each day). Research it before the decision on the way to connect.

## 6. Open decisions for the grilling

1. Which way to connect (recommended: join code and helper server on Cloudflare).
2. Where Host and Join go: Bosman Edition Options, the start screen, or the original's network game screens.
3. Who runs and pays for the helper server.
4. How many players a game takes.

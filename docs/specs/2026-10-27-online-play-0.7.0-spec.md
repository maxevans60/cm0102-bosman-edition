# Easy online play (0.7.0, #45)
Status: Ready

Interview: 2026-10-27, Q1 to Q30, all settled. Background and costs: `docs/research/2026-10-19-easy-network-play-plan.md`. Terms: the "Network play" section of `GLOSSARY.md`.

## Problem

A network game works today only after about ten manual steps on Windows: compatibility mode, administrator rights, a VPN program, a hand-edited address file, a network patch and a firewall question. The forum thread on network play is six years of players who are stuck at one of these steps. In Bosman Edition, network games do not work at all, because the port answers every network call with "no network". Mac and Linux players cannot play network games.

## Solution

A player clicks Network Play, then Join Network Game. The game list shows every open network game online and on the LAN. The player clicks one, clicks Join, picks a club and plays. There is no patch, no VPN, no router setting and no IP address. Mac, Windows and Linux players can play in one game. It costs Max nothing on the Cloudflare free plan.

Only the network layer under the game changes. The original's network screens, packets and host logic run as they are. A free lobby on Cloudflare keeps the list of online games, and a relay on Cloudflare passes the traffic, so no router setting is needed.

## User stories

1. As a human manager, I want to see every open online game in the game list, so that I can join one with no setup.
2. As a human manager, I want LAN games and online games in the same list, so that one screen covers both.
3. As a host, I want my game to show in the lobby when I start a network game, so that my friends can find it.
4. As a host, I want to set a password on my game, so that only my friends can join.
5. As a human manager on a Mac, I want to join a game that a Windows host runs, so that the platform does not matter.
6. As a human manager on a different Bosman Edition version, I want a message that names the host's version, so that I know what to update.
7. As a human manager whose connection drops, I want my club to go on holiday and show Away, and I want to take it back with my manager password when I return.
8. As a human manager, I want the game to tell me when the host quits, and I want to go back to the start screen, while the host's game saves.
9. As a host, I want to leave an always-on game open at home, so that friends can join and leave in the evening. The game moves forward only when a human manager clicks Continue.
10. As a human manager, I want the game list to show the season, the day and the managers online for each game, so that I can see which game is active.
11. As a human manager, I want an invite link that I can post on Discord or the forum, so that a friend clicks it and joins.
12. As a friend who does not have the game, I want the invite link to open a page that says what the game is and how to get it.
13. As a human manager, I want a clear reason and a next step when something fails, in place of "The network game could not be created" or a freeze at "Contacting Server".
14. As a human manager, I want a connection test in Network Play that says online play works or what is wrong.
15. As a human manager, I want to see each human manager's ping, so that I can see if the network is the cause of a slow game.
16. As a host, I want Kick, so that I can remove a human manager from a public game.
17. As a player who does not want online games, I want to turn Online play off on the options screen, so that the game does not contact the lobby.
18. As a player, I want LAN and VPN games to work if the online service is down or full for the day.
19. As a player, I want the lobby to keep no record of my internet address.
20. As a host of a restored save, I want the lobby to stop a second copy of the same game, so that the league does not split into two games.

## Decisions

Design:

- Change only the network layer under the game. The original's network screens, packets, host logic, holiday code and manager passwords run unchanged. Single-player games do not run this code, so the same-save rule (Q152) holds.
- Real sockets replace the "no network" answers for all 18 network calls the game imports. The game gets its own addresses directly in place of the original's address file lookup, which starts a background program that the port does not have.
- The host's game tells the lobby about its game and sends a heartbeat. The lobby drops a game that stops its heartbeat.
- When the game list asks for games, the network layer answers with the lobby's games as well as the LAN games.
- When a human manager joins an online game, the network layer carries the connection through the relay over a secure web connection on port 443. The game sees an ordinary connection.
- The game sends the host's packets to the relay in batches, to stay inside the free plan.

Rules from the interview:

- 0.7.0 ships real sockets, the lobby and the relay together (Q2).
- No new screens for the lobby. The game list is the original's Select Network Game screen. There is no waiting room and no chat (Q9). The original's names Network Play and Join Network Game stay (Q27).
- Small additions that need a message or field: the game password, Kick, the error messages, the connection test and the ping.
- All games are public in the list. A host can set an optional password (Q10).
- Up to 16 human managers in a game, the original's limit (Q4).
- The game list holds up to 50 games with the original's scrollbar. The recompiler raises the original's limit of 9 (research note, section 10). A test fills the list with 50 games (Q31, Q33).
- On a fast connection, the runtime reports the mouse button as up to a joiner's game after it sends a click, until the real button comes up. The game's input code does not change (Q34).
- Tests on one Mac join the host through the Mac's LAN address. The loopback test 0x8af290 stays as it is (Q32).
- Mac, Windows and Linux players play in one game (Q5).
- A player on a different version cannot join. The message names the host's version (Q6).
- When the host quits or drops, the host's game saves at once, and each human manager sees "The host left the game" and goes back to the start screen (Q7).
- Drop in and drop out: a human manager who leaves is Away, and their club is on holiday as in the original. They take it back with their manager password (Q15).
- Always-on game: it moves forward only when a human manager clicks Continue, as in the original. The game list shows the season, the day and the managers online (Q16).
- Invite link: a web link on the lobby address opens a page with an "Open in Bosman Edition" button and the game code. The game registers a `cm0102://` link on Mac, Windows and Linux. A game with a password still asks for it (Q17).
- The lobby allows one open copy of a game. A second host sees that the game is already open and can join it (Q23).
- Player names are free text with no account. A word filter checks player names and game names (Q13).
- The host has Kick. The lobby allows at most 2 open games for each internet address (Q14).
- The lobby keeps internet addresses in memory only while the game is open and writes them to no log. The site says this in one line (Q26).
- Online play is an option on the game's options screen, on by default. When it is off, the game list shows LAN games only and the game does not contact the lobby (Q25).

Server:

- The lobby and relay run on Max's Cloudflare account on the free plan, on a `workers.dev` address. The code lives in the repo under `server/` (Q11).
- When the free allowance runs out, the game list says "Online games are full today, try again later" until 00:00 UTC. LAN and VPN play still work. The game never crashes on a server failure (Q12).
- The relay stores no game data.
- Max deploys the server. A deploy waits for his yes.

Build:

- Slices build in worktrees while the 0.6.0 gates run, with `build-slot.sh` for builds. Nothing merges to main before 0.6.0 ships (Q3).
- The build sessions may start, use and suspend the "Windows 11 (1)" Parallels virtual machine, after the 0.6.0 gates finish (Q30).
- Never use patch code. Work from the original's own network code and the function map. Read each cited function and cite its address (repo rule).

## Testing

Seams:

1. Two-game run. A script starts a host game and a joining game with timed clicks, joins through Network Play and compares screen dumps. It uses the same timed-click method as the smoke tests. It runs against 127.0.0.1, then a local copy of the server, then the real server.
2. Server tests in `server/`, against a local copy of the lobby and relay.
3. The four same-save sets, unchanged, with networking compiled in.

Release tests (Q29), because Max cannot run a test evening:

1. Two and four games on this Mac play through the real lobby and relay.
2. The Windows 11 virtual machine joins a game that this Mac hosts.
3. GitHub's Windows and Linux machines join a game that this Mac hosts over the internet, with scripted clicks.
4. A four-player game runs through a World Cup, a Euros and every season change between them, with a drop and rejoin, a host quit and a version mismatch.

The one gap: a home router with strict settings.

## Out of scope

Derby night, chat, the weekly challenge, the hall of fame, deadline day, Discord in any form, pass the host, spectators, direct connections without the relay, accounts, ban lists, a cloud host that runs the game.

## Open decisions

None.

## Slices

### 1. Read the original's network code
Blocked by: none
Delivers: a research note in `docs/research/` that cites each network function by address. It covers the packets and the list broadcast, whether the game waits on each call or polls, the address lookup and how to replace it, whether a joining human manager needs the database, the join and holiday flow, and an estimate of traffic. No game code.
Merge danger: two-way door. Blast radius: none, nothing ships.
- [ ] Every claim cites a function address that the author read.
- [ ] The note lists what slice 2 must change and what it must not touch.

### 2. Real sockets
Blocked by: 1
Delivers: LAN and VPN play on Mac, Windows and Linux. The game finds its own addresses directly. Two games on this Mac play together through 127.0.0.1, and the Windows virtual machine joins a game on this Mac. Traffic for 2 and 4 human managers is logged.
Merge danger: two-way door. Blast radius: network games only. The same-save sets prove that single-player games do not change.
- [x] Two-game run passes, through this Mac's LAN address in place of 127.0.0.1 (Q32), with 1 and 3 joiners, and under wine with the Windows build (`tests/smoke/network_run.py`).
- [x] Same-save sets pass.
- [x] Traffic log for 2 and 4 human managers is in the research note (section 11).
- [ ] The Windows virtual machine joins a game on this Mac: waits for the 0.6.0 gates (Q30).

As built (2026-10-27, branch `claude/online-s2-sockets-b40614`): the merge danger stays a two-way door. Files: `src/recomp_rt/win/net.c`, `net_core.c`, `src/recomp_rt/host/net_posix.c`, `net_win32.c`, `host_net.h`, `CMakeLists.txt`, `packaging/windows/windows.cmake` (links ws2_32 and iphlpapi) and `tools/recomp/widescreen.py` (four entries inside 0x8aee20 removed). A merge conflict is likely only in `CMakeLists.txt` and `widescreen.py`. Open: on a fast connection one click of a human manager acts on several host screens, because the original's client loop sends the click again for each new screen while the button is down (research note, section 11). It must be fixed before 0.7.0 ships, and where to fix it needs a decision. Also open for slice 4: the game list shows one row for each adapter of the host.

### 3. Lobby and relay server
Blocked by: none
Delivers: the Worker and relay in `server/` with tests: register, heartbeat, list, relay, the free-plan limits, the "full today" answer, at most 2 games for each address, one open copy of a game, the word filter and addresses in memory only. Max deploys it.
Merge danger: two-way door, but public, so the Opus review rule applies. Blast radius: the online list is down. LAN play still works.
- [ ] Server tests pass against the local copy.
- [ ] No log line or stored record holds an internet address.
- [ ] Max approves the deploy.

### 2b. One click acts on one screen
Blocked by: 2
Delivers: a joiner's click acts on one host screen only, on a fast LAN and through the relay (Q34, research note section 11).
Merge danger: two-way door. Blast radius: joiners' clicks in network games. Single-player input does not change.
- [x] The two-game run clicks Next on Enter Name and lands on the next screen only.
- [x] Same-save sets pass.

As built (2026-10-27, branch `claude/online-s2-click-fix`): the merge danger stays a two-way door. Files: `src/recomp_rt/win/user32.c` (the rule in the input poll wrapper, only at the poll 0x7fdf17 of the client loop), `src/recomp_rt/win/net_core.c` and `net_core.h` (`nc_resets_in`, the count of host screens read), `tests/smoke/network_run.py`. In a press, the client loop gets a widget once, and a second widget only while no new host screen came (a press that ends a text entry gives the field, then the button). Mac and wine runs pass with 1 and 3 joiners. Not run: a click through the relay (slice 5). Research note, section 11.

### 4a. A game list of 50 games
Blocked by: 2
Delivers: the game list holds up to 50 games with the original's scrollbar (Q31, Q33). Constant patches in the recompiler (`tools/recomp/game_list.py`) and a runtime buffer for the screen's copy of the names. No game function is replaced.
Merge danger: two-way door. Blast radius: the game list and join in network games.
- [x] Game list run passes: 50 ACKs fill the list, 5 BYEs remove games, and a join of the last row reaches its host (`tests/smoke/game_list_run.py`, under wine with `game_list_wine.sh`).
- [x] Two-game run passes.
- [x] ctest and the same-save sets pass.

As built (2026-10-27, branch `claude/online-s4a-list50-211ec8`): the merge danger stays a two-way door. Files: `tools/recomp/game_list.py` (24 constant sites), `tools/recomp/widescreen.py` (the import), `src/recomp_rt/win/game_list.c` and `src/recomp_rt/rt.h` (the buffer of the names), `tests/smoke/game_list_run.py` and `game_list_wine.sh`. Each array has 51 slots, 50 games and the original's spare slot (research note, section 10). The game list run fails on the slice 2b build (9 games) and passes on this one. The two-game run, ctest (50 tests) and the four same-save sets pass, with the slice 2b build as `ref-build`. In the match set, `condition-on` and `condition-off` differed once under load and were equal in a run of the two alone. For a merge with slice 4b: the patches act only while 0x77a890, 0x77a9f0, 0x77af20 and 0x81f8d0 stay translated. If slice 4b replaces one of them with `RT_REPLACE`, the build stops with "no translated instruction" at those sites (the build runs `recomp.py --all`), and the replacement must use the new layout.

### 4. Online games in the game list
Blocked by: 2b, 3
Delivers: a host puts its game in the lobby. The game list shows online and LAN games with the season, the day and the managers online, and one row for each host. Also the Online play option, the version check, the game password and the "full today" message.
Merge danger: two-way door. Players see it, so the Opus review rule applies.
- [x] Two-game run passes against the local server.
- [x] With Online play off, the game makes no lobby request.
- [x] Same-save sets pass.

As built (2026-10-27, branch `claude/online-s4b-lobby-ea2663`, less the 50-game list of slice 4a): the merge danger stays a two-way door. Files: `src/recomp_rt/win/net_lobby.c` (new), `lobby.c`, `lobby_http.c`, `lobby_rows.c` and their headers (new), `net_core.c` and `net_core.h` (hooks, join check, `nc_peers`), `net.c`, `win.h`, `options.c`, `options.h`, `options_screen.c` (a third tab, Network), `host/net_posix.c`, `host/net_win32.c`, `host_net.h` (thread, lock, name look-up, route address), `CMakeLists.txt` (Threads, the `lobby` test), `tests/net/test_lobby.c`, `tests/options/test_options.c`, `tests/smoke/network_run.py` (`--lobby`), `tests/smoke/smoke_run.py` (online-on, online-off). Merge with slice 4a: `LR_CAP` in `lobby_rows.h` must become 50 when its cap of 0x77b0eb does; a conflict is likely only in `CMakeLists.txt` and `smoke_run.py`. Tests: ctest 51 of 51; the two-game run with `--lobby` and with 3 joiners; the four same-save sets (start.sav, prematch.sav, a new game, yeovil.sav with `--days 7`, against `ref-builds/main-batchAX`, with online-on and online-off) and the options box set pass. The sets ran before the second review round, which changed only network code; start.sav and the options set passed again on the final build. Research note, section 12. Open: the screens of the game password (Max's choice), the lobby address of the release, the game id of a restored save, and the lobby's memory (section 12, finding for the server).

### 5. Join through the relay
Blocked by: 4
Delivers: a human manager on another network joins with no router setting. Adds a library for secure web connections on three platforms.
Merge danger: two-way door. Blast radius: online joins fail. A new library ships on three platforms.
- [ ] Two-game run passes through the real relay.
- [ ] A GitHub Windows machine and a GitHub Linux machine join a game on this Mac.

As built so far (2026-10-28, branch `claude/online-s5-relay-16166c`, not finished): the merge danger stays a two-way door. Files: `src/recomp_rt/win/relay.c`, `websocket.c`, `secure.c`, `tls_config.h` and their headers (new), `net_core.c` and `net_core.h` (route and incoming hooks), `net_lobby.c`, `lobby.c`, `lobby_http.c` (https), `lobby_rows.h` (`LR_CAP` 50, `LR_RELAY_PEERS`), `host/net_posix.c`, `host/net_win32.c`, `host_net.h`, `CMakeLists.txt` (mbedTLS 3.6.7), `packaging/windows/windows.cmake` (crypt32), `tests/net/test_relay.c`, `tests/net/tls_check.py`, `tests/smoke/network_run.py` (`--relay`, `--remote`, `--game-name`), `tests/smoke/relay_join_remote.py`, `.github/workflows/online-join.yml`. The two-game run with `--relay` passes through the local server's relay, and ctest passes. Not run yet: the relay with 3 joiners, the real relay, the GitHub machines, the four same-save sets. Research note, section 13.

### 6. Drop in, drop out and host quit
Blocked by: 5
Delivers: Away status, rejoin with the manager password, the host's save and message when it quits, and Kick.
Merge danger: two-way door. Blast radius: a human manager can lose their club in a network game.
- [ ] A dropped human manager rejoins the same club.
- [ ] A host quit saves and sends every human manager to the start screen.
- [ ] Kick removes a human manager, and their club goes on holiday.

### 7. Clear errors, connection test, ping
Blocked by: 6
Delivers: each failure shows a reason and a next step. Network Play has a connection test. The game list and the game show each human manager's ping.
Merge danger: two-way door.
- [ ] Each failure in a list of forced failures (lobby down, relay down, host gone, version mismatch, wrong password, full today) shows its message.
- [ ] The connection test reports each forced failure.

### 8. Invite links
Blocked by: 5
Delivers: the join web page on the lobby address and the `cm0102://` link on Mac, Windows and Linux.
Merge danger: one-way door on players' computers, because the game registers a link handler. The Opus review rule applies.
- [ ] A click on the link joins the game on each platform.
- [ ] The page works for a visitor who does not have the game.

### 9. Full online test
Blocked by: 6, 7, 8
Delivers: the four release tests in Testing, with results in `docs/TESTING.md`.
Merge danger: two-way door. Tests only.
- [ ] All four release tests pass, and every failure is fixed and tested again.

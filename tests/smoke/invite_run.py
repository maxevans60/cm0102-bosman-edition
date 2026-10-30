#!/usr/bin/env python3
"""Invite links join the game (0.7.0, Q17, #45): a host in the lobby, and a joiner that gets cm0102://join/<code>.

Usage:
    python3 tests/smoke/invite_run.py <cm0102_recomp> <game folder> [--lobby-url URL] [--shots <folder>]
        [--slow k] [--only a,b]

It uses the timed clicks of tests/smoke/network_run.py and a local copy of the lobby (server/, `npx wrangler dev`),
or --lobby-url. The joiner sees lobby rows only (RECOMP_TEST_NO_LAN_ROWS), so it joins through the relay. The
joiner clicks nothing on the way: the runtime walks the original's screens (src/recomp_rt/win/invite.c), Network
Play, Join Network Game, then the game's row and Next. Each case runs one host and one joiner, one case after the
other. Two network runs on one computer collide on port 6667: start it through
/Users/max/Projects/cm0102/lead-notes/tools/net-lock.sh.

Cases:
    link      the joiner starts with the link as its argument (Windows and Linux; the Mac's Apple event goes to the
              same call). It joins, plays Enter Name and Select Nationality, and its screens equal the host's.
    running   the joiner runs already, on the start screen, and a second copy starts with the link: the second
              copy gives the link to the first and ends (host_link_pass), as a click on a link does while the
              game runs. The first copy joins as above.
    password  the host's game has the password "secret": the link opens the password box of slice 6, the joiner
              types the password and joins.
    gone      the link names a game code that is not open: the joiner's "Unable To Join" box says that the game
              is no longer in the list.
"""
import json
import os
import subprocess
import sys
import tempfile
import time
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import network_run as nr  # noqa: E402
import password_run as pr  # noqa: E402
import smoke_run  # noqa: E402

LIST = "wait screen:0x81f8d0"           # the game list, which the runtime opens


def joiner_steps(n, password=False):
    """The joiner: the start screen, then nothing until the game list; then the steps after the join."""
    steps = nr.START[:-1] + [LIST, "3000 dump {d}/join%d-0-list.bmp" % n]
    if password:
        return steps + [
            "9000 dump {d}/join%d-1-box.bmp" % n,
            f"9500 {pr.FIELD}",
            "10000 type secret",
            f"11500 {pr.OFF_FIELD}",
            f"12500 {pr.JOIN}",
            "18000 dump {d}/join%d-1-name.bmp" % n,
            "19000 quit",
        ]
    tail = nr.joiner_steps(None, n, by_row=True)
    return steps + tail[tail.index("11000 dump {d}/join%d-1-name.bmp" % n):]


def lobby_code(url, name, timeout):
    end = time.time() + timeout
    while time.time() < end:
        for g in nr.lobby_games(url) or []:
            if g.get("name") == name:
                return g["code"]
        time.sleep(0.5)
    return None


def main(argv):
    args = argv[1:]
    if len(args) < 2:
        print(__doc__)
        return 2
    program, game = os.path.abspath(args[0]), os.path.abspath(args[1])
    lobby_url, shots, slow, only = None, None, 1.0, None
    rest = args[2:]
    while len(rest) >= 2:
        k, v = rest.pop(0), rest.pop(0)
        if k == "--lobby-url":
            lobby_url = v
        elif k == "--shots":
            shots = os.path.abspath(v)
        elif k == "--slow":
            slow = float(v)
        elif k == "--only":
            only = v.split(",")
        else:
            print(__doc__)
            return 2
    failed = []

    def check(ok, what):
        print(("ok   " if ok else "FAIL ") + what)
        if not ok:
            failed.append(what)

    lobby_proc = None
    if not lobby_url:
        port = 8690 + os.getpid() % 100
        lobby_proc = nr.start_lobby(port)
        if not lobby_proc:
            print("FAIL the local lobby (npx wrangler dev in server/) does not answer")
            return 1
        lobby_url = f"http://127.0.0.1:{port}"
    env = {"RECOMP_LOBBY_URL": lobby_url, "RECOMP_TEST_NO_LAN_ROWS": "1"}
    shots = shots or tempfile.mkdtemp(prefix="cm0102-invite-")
    print(f"lobby {lobby_url}, shots {shots}")
    cases = [c for c in ("link", "running", "password", "gone") if not only or c in only]
    try:
        for case in cases:
            run(case, program, game, lobby_url, env, os.path.join(shots, case), slow, check)
    finally:
        if lobby_proc:
            os.killpg(lobby_proc.pid, 9)
    print("invite run: " + ("PASS" if not failed else f"FAIL ({len(failed)})"))
    return 1 if failed else 0


def run(case, program, game, lobby_url, env, shots, slow, check):
    os.makedirs(shots, exist_ok=True)
    work = tempfile.mkdtemp(prefix="cm0102-invite-work-")
    name = "Invite" + case.capitalize()
    nr.GAME_NAME = name
    host_steps = pr.host_steps() if case == "password" else nr.host_steps(1)
    host_steps = [s.replace("type Bosman", f"type {name}") for s in host_steps]
    stop = __import__("threading").Event()
    __import__("threading").Thread(target=nr.keep_awake, args=(lobby_url, stop), daemon=True).start()
    host = join = second = None
    errs = []
    try:
        if case != "gone":
            host, e = nr.start(program, game, work, "host", host_steps, shots, slow, {"RECOMP_LOBBY_URL": lobby_url})
            errs.append(e)
            ready = nr.wait_for(os.path.join(shots, "host-stderr.txt"), f"screen {nr.NAME_BUILD}", host, 300 * slow)
            check(ready, f"{case}: the host reaches its Enter Name screen")
            code = lobby_code(lobby_url, name, 30 * slow) if ready else None
            check(code is not None, f"{case}: the lobby lists the host's game ({code})")
        else:
            code = "ZZZZ2222"
        if code:
            link = f"cm0102://join/{code}"
            steps = joiner_steps(1, password=case == "password")
            if case == "gone":
                steps = nr.START[:-1] + [LIST, "9000 dump {d}/join1-1-box.bmp", "10000 quit"]
            join, e = nr.start(program, game, work, "join1", steps, shots, slow, env,
                               args=() if case == "running" else (link,))
            errs.append(e)
            if case == "running":
                at_start = nr.wait_for(os.path.join(shots, "join1-stderr.txt"), "reached setup", join, 120 * slow)
                check(at_start, "running: the joiner reaches the start screen")
                time.sleep(2)
                user = os.path.join(work, "join1")
                e2 = open(os.path.join(shots, "second-stderr.txt"), "w")
                errs.append(e2)
                second = subprocess.Popen([program, "--game", game, link], stdout=subprocess.DEVNULL, stderr=e2,
                                          env=dict(os.environ, HOME=user, RECOMP_USER_DIR=user, SDL_VIDEODRIVER="dummy",
                                                   SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1", **env))
                second.wait(timeout=60)
                check(second.returncode == 0 and "the copy of the program that runs took the link" in
                      open(os.path.join(shots, "second-stderr.txt"), errors="replace").read(),
                      "running: the second copy gives the link to the first and ends")
            join.wait(timeout=240 * slow)
        if host:
            host.wait(timeout=300 * slow)
    except subprocess.TimeoutExpired:
        pass
    finally:
        for p in (join, host, second):
            if p and p.poll() is None:
                p.kill()
                p.wait()
        for e in errs:
            e.close()
        stop.set()
    jerr = open(os.path.join(shots, "join1-stderr.txt"), errors="replace").read() \
        if os.path.exists(os.path.join(shots, "join1-stderr.txt")) else ""
    dump = lambda f: os.path.join(shots, f)
    if join:
        check(join.returncode == 0, f"{case}: the joiner ended with code {join.returncode}")
    if host:
        check(host.returncode == 0, f"{case}: the host ended with code {host.returncode}")
    check("recomp: invite: the game opens Join Network Game" in jerr and 'recomp: invite: Join "' in jerr,
          f"{case}: the runtime opened the game list and pressed Join")
    if case in ("link", "running"):
        check("recomp: relay: joined" in jerr, f"{case}: the joiner joined through the relay")
        for host_shot, join_shot, what in (("host-1-name.bmp", "join1-1-name.bmp", "Enter Name"),
                                            ("host-3-nation.bmp", "join1-3-nation.bmp", "Select Nationality")):
            if os.path.exists(dump(host_shot)) and os.path.exists(dump(join_shot)):
                share = smoke_run.differing_share(dump(host_shot), dump(join_shot))
                check(share <= 0.01, f"{case}: the joiner's {what} equals the host's: {share:.2%} differ")
            else:
                check(False, f"{case}: {what}: dump missing")
        peer = next((p for p in nr.connections(dump("join1-net.log")) if p.startswith("0.66.")), None)
        clicks = nr.click_packets(dump("join1-net.log"), peer) if peer else None
        check(clicks == nr.JOINER_CLIENT_CLICKS, f"{case}: the joiner sent {clicks} click packets")
    elif case == "password":
        check("recomp: password box opened" in jerr and "recomp: password box: the join runs again" in jerr,
              "password: the password box opened and the join ran again")
        check("recomp: relay: joined" in jerr, "password: the joiner joined through the relay")
        if os.path.exists(dump("host-1-name.bmp")) and os.path.exists(dump("join1-1-name.bmp")):
            share = smoke_run.differing_share(dump("host-1-name.bmp"), dump("join1-1-name.bmp"))
            check(share <= 0.01, f"password: the joiner's Enter Name equals the host's: {share:.2%} differ")
        else:
            check(False, "password: Enter Name: dump missing")
    else:
        check("join box: This network game is no longer in the list" in jerr,
              "gone: the box says that the game is no longer in the list")


if __name__ == "__main__":
    sys.exit(main(sys.argv))

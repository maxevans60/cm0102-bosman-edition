#!/usr/bin/env python3
"""Two-game run (0.7.0 slice 2, #45): a host game and a joining game on this computer play one network game.

Usage:
    python3 tests/smoke/network_run.py <cm0102_recomp> <game folder> [--address a.b.c.d] [--shots <folder>]
        [--joiners n] [--slow k]

The games use the same timed clicks as the smoke run (RECOMP_SCRIPT), each with its own empty user folder.

The host: Network Play, Start Network Game, the game name "Bosman", Select League(s) Next (the original makes
the network game here, setup_create_or_join_network_game 0x8388e0, and loads the database), then its own Enter
Name screen (0x820f50). The run starts the joiners when the host's screen log reaches that screen.

Each joiner: Network Play, Join Network Game, the game list (0x81f8d0), types the host's address in the Join
field and clicks Next. Max's decision Q32: the games meet through this computer's LAN address (--address,
default: the address of the default route), not 127.0.0.1, because the original's loopback test 0x8af290 holds
127.0.0.1:6667. The host opens Enter Name on the joiner's channel (setup_on_network_player_joined 0x838a80)
and sends it. The mouse-up of the join click reaches the joiner's client loop 0x7fdc00 and goes to the host as
a click on Next, so the host answers "Names Are Blank" and, after Ok, Manager Status. The joiner clicks Add
Manager, gets Enter Name, and types "Join" and "Two". After the joiners quit, the host's own manager types the
same names on its own Enter Name.

A click on a host screen is not followed by a dump of the next screen: while the button is down, the client
loop sends the click again when the host's next screen arrives (poll_input 0x55f620 at 0x7fdf17). On a fast
connection one click then acts on two screens. See the research note, section 10.

Checks:
    1 each game ends with code 0
    2 the joiner's Enter Name equals the host's own Enter Name (at most 1% of pixels differ: the text cursor)
    3 the joiner's Enter Name with its typed names, drawn by the host, equals the host's own with the same names
    4 the traffic logs (RECOMP_NET_LOG) show the connection on both sides, with packets both ways

--slow k multiplies every step time and timeout by k, for the Windows build under wine in an emulated container.

--joiners n (default 1, at most 3) starts n joiners at once, for the traffic of n + 1 human managers. The run
prints each game's traffic log. With --shots the dumps and logs stay in that folder.
"""
import os
import shutil
import socket
import subprocess
import sys
import tempfile
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import smoke_run  # noqa: E402

START = [
    smoke_run.INTRO_CLICK,
    smoke_run.SETTINGS_WAIT,
    "8200 click 522 272",                   # settings: Background Changes Off
    "8500 click 662 524",                   # settings: Ok
    "wait setup",
    "1000 click 277 346",                   # Network Play
]
NAME_BUILD = "0x820f50"                     # Enter Name of a new human manager (setup_add_human_manager)
HOST = START + [
    "3500 click 277 177",                   # Start Network Game
    "6500 type Bosman",                     # Enter Network Game Name
    "8500 click 705 572",                   # Next (ends the text entry)
    "10000 click 705 572",                  # Next
    "wait leagues",
    "2000 click 705 572",                   # Select League(s): Next makes the network game and loads the data
    f"wait screen:{NAME_BUILD}",
    "2000 dump {d}/host-1-name.bmp",
    # The joiners play now. The host's own manager types the same names after they quit.
    "60000 click 560 246",                  # First Name
    "60500 type Join",
    "62000 click 560 310",                  # Second Name
    "63000 type Two",
    "65000 dump {d}/host-2-typed.bmp",
    "66000 quit",
]


def joiner_steps(address, n):
    return START + [
        "3500 click 612 243",               # Join Network Game
        "wait screen:0x81f8d0",             # the game list; the Join field takes the keys
        "3000 dump {d}/join%d-0-list.bmp" % n,
        f"3500 type {address}",
        "6000 click 705 572",               # Next (ends the text entry)
        "7500 click 705 572",               # Next: connect, then run_network_client 0x7fdc00
        # The mouse-up of that click reaches the client loop, which sends it to the host as a click on Next of
        # Enter Name (Display::poll_input 0x55f620 at 0x7fdf17). So the host answers "Names Are Blank". This is
        # the original's input code, not the network layer.
        "11000 dump {d}/join%d-blank.bmp" % n,
        "11500 click 400 572",              # Ok: the host opens Manager Status on the joiner's channel
        "13000 dump {d}/join%d-status.bmp" % n,
        "13500 click 45 120",               # Add Manager (left bar): Enter Name
        "16000 dump {d}/join%d-1-name.bmp" % n,
        "16500 click 560 246",              # First Name
        "17000 type Join",
        "19000 click 560 310",              # Second Name
        "20000 type Two",
        "22000 dump {d}/join%d-2-typed.bmp" % n,
        "23000 quit",
    ]


def lan_address():
    """The address of the default route (no packet is sent)."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("192.0.2.1", 9))
        return s.getsockname()[0]
    finally:
        s.close()


def start(program, game, work, name, steps, shots, slow=1):
    user = os.path.join(work, name)
    os.makedirs(user)
    with open(os.path.join(user, "settings.txt"), "w") as f:
        f.write("display = classic\n")
    steps = smoke_run.scaled_steps("classic", shots, steps)
    if slow != 1:   # --slow: every step time and wait limit (120 s by default) times this factor
        steps = [f"{s} {int(120000 * slow)}" if s.startswith("wait") else
                 " ".join([str(int(int(s.split()[0]) * slow))] + s.split()[1:]) for s in steps]
    env = dict(os.environ, HOME=user, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy", RECOMP_NO_BOX="1",
               RECOMP_USER_DIR=user, RECOMP_SCREEN_LOG="1",
               RECOMP_NET_LOG=os.path.join(shots, f"{name}-net.log"), RECOMP_SCRIPT="; ".join(steps))
    err = open(os.path.join(shots, f"{name}-stderr.txt"), "w", errors="replace")
    return subprocess.Popen([program, "--game", game], env=env, stdout=subprocess.DEVNULL, stderr=err), err


def wait_for(path, text, proc, timeout):
    end = time.time() + timeout
    while time.time() < end and proc.poll() is None:
        with open(path, errors="replace") as f:
            if text in f.read():
                return True
        time.sleep(0.5)
    return False


def connections(log):
    """{peer: last line's fields} of the 'end' and 'close' lines of a traffic log."""
    out = {}
    if not os.path.exists(log):
        return out
    for line in open(log):
        p = line.split()
        if len(p) > 2 and p[1] in ("close", "end"):
            f = dict(x.split("=", 1) for x in p if "=" in x)
            nums = [int(x) for x in p if x.isdigit()]
            # out=B bytes P packets S sends in=B bytes P packets
            f["bytes_out"], f["packets_out"], f["sends"], f["packets_in"] = int(f["out"]), nums[0], nums[1], nums[2]
            f["bytes_in"] = int(f["in"])
            out[f["peer"]] = f
    return out


def main(argv):
    args = argv[1:]
    if len(args) < 2:
        print(__doc__)
        return 2
    program, game = os.path.abspath(args[0]), os.path.abspath(args[1])
    address, shots, joiners, slow = lan_address(), None, 1, 1.0
    rest = args[2:]
    while rest:
        k, v = rest[0], rest[1]
        rest = rest[2:]
        if k == "--address":
            address = v
        elif k == "--shots":
            shots = os.path.abspath(v)
        elif k == "--slow":
            slow = float(v)
        elif k == "--joiners":
            joiners = max(1, min(3, int(v)))
        else:
            print(__doc__)
            return 2
    failed = []

    def check(ok, text):
        print(("ok   " if ok else "FAIL ") + text)
        if not ok:
            failed.append(text)

    with tempfile.TemporaryDirectory(prefix="cm0102-network-") as work:
        shots = shots or work
        os.makedirs(shots, exist_ok=True)
        print(f"host address {address}")
        host, host_err = start(program, game, work, "host", HOST, shots, slow)
        ready = wait_for(os.path.join(shots, "host-stderr.txt"), f"screen {NAME_BUILD}", host, 240 * slow)
        check(ready, "the host reaches its Enter Name screen with the network game made")
        procs = []
        if ready:
            for n in range(1, joiners + 1):
                # The original's loopback test 0x8af290 binds 127.0.0.1:6667 for a moment in each game. Two games
                # on one computer that run it at the same time collide, and one turns network play off.
                if n > 1:
                    time.sleep(4 * slow)
                procs.append(start(program, game, work, f"join{n}", joiner_steps(address, n), shots, slow))
        for p, err in procs:
            try:
                p.wait(timeout=180 * slow)
            except subprocess.TimeoutExpired:
                p.kill()
                p.wait()
            err.close()
        try:
            host.wait(timeout=240 * slow)
        except subprocess.TimeoutExpired:
            host.kill()
            host.wait()
        host_err.close()

        check(host.returncode == 0, f"host ended with code {host.returncode}")
        for n, (p, _) in enumerate(procs, 1):
            check(p.returncode == 0, f"joiner {n} ended with code {p.returncode}")
        dump = lambda name: os.path.join(shots, name)
        for n in range(1, len(procs) + 1):
            for host_shot, join_shot, what in (("host-1-name.bmp", f"join{n}-1-name.bmp", "Enter Name"),
                                                ("host-2-typed.bmp", f"join{n}-2-typed.bmp", "Enter Name with the typed names")):
                if os.path.exists(dump(host_shot)) and os.path.exists(dump(join_shot)):
                    share = smoke_run.differing_share(dump(host_shot), dump(join_shot))
                    check(share <= 0.01, f"joiner {n}'s {what} equals the host's: {share:.2%} of pixels differ")
                else:
                    check(False, f"joiner {n}'s {what}: dump missing")

        host_log = connections(os.path.join(shots, "host-net.log"))
        joined = [f for f in host_log.values() if f["peer"].split(":")[0] == address and not f["peer"].endswith(":6667")]
        check(len(joined) == len(procs) and all(f["packets_in"] and f["packets_out"] for f in joined),
              f"host log: {len(joined)} joiner connections with packets both ways")
        for n in range(1, len(procs) + 1):
            log = connections(os.path.join(shots, f"join{n}-net.log"))
            c = log.get(f"{address}:6667")
            check(bool(c and c["packets_in"] and c["packets_out"]), f"joiner {n} log: the connection to {address}:6667")
        for name in ["host"] + [f"join{n}" for n in range(1, len(procs) + 1)]:
            path = os.path.join(shots, f"{name}-net.log")
            if os.path.exists(path):
                print(f"--- {name} traffic")
                print(open(path).read().rstrip())
        for f in sorted(os.listdir(shots)) if shutil.which("sips") else []:
            if f.endswith(".bmp"):
                subprocess.run(["sips", "-s", "format", "png", dump(f), "--out", dump(f[:-4] + ".png")],
                               capture_output=True)
    print("network run: " + ("PASS" if not failed else f"FAIL ({len(failed)})"))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))

#!/usr/bin/env python3
"""A joiner's tactics drag in a network game, in both display modes (0.7.0 item 212).

Usage:
    python3 tests/smoke/network_drag_run.py <cm0102_recomp> <game folder> --out <folder> [--slow k]

Four LAN games, Online play off, one for each display of the host and the joiner (classic, widescreen). In each, the
host makes the network game and takes Arsenal (tests/smoke/dropin_run.py), the joiner joins, takes the second club
(Barnsley, 4-4-2) and both click Finish. After the first news the joiner opens its squad's Tactics, drags the left MC
(575, 340 on 800x600) up to (575, 270), and opens Tactics again from the squad screen. The drop reaches the host in
the joiner's click: the Display's drag rectangle and the pitch widget (net_scale.c). The host's tactics input then
places the player.

Checks, for each joiner display:
    1 every game ends with code 0
    2 the pitch after the drag differs from the pitch before it (the player moved)
    3 the pitch after the drag, and on the Tactics opened again (the host's data), is the same under a classic host and
      under a widescreen host
Before the fix a widescreen joiner's drag on a classic host left the player at MC, and any drag on a widescreen host
made him FL. Each run keeps its dumps and logs in <folder>/<host>-<joiner>.
"""
import os
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import network_run as nr  # noqa: E402
import dropin_run as d  # noqa: E402
import smoke_run  # noqa: E402

PITCH = {"classic": (452, 152, 778, 543), "widescreen": (724, 203, 1244, 724)}
DRAG = (575, 340, 575, 270)


def steps():
    host = d.host_steps(100000)
    host = host[:-1] + ["240000 dump {d}/host-4-late.bmp", "250000 quit"]       # stay in the game
    join = d.join_steps(1, nr.lan_address(), quit_ms=10 ** 9)
    cut = next(i for i, s in enumerate(join) if "join1-6-status" in s)
    t = int(join[cut].split()[0])
    join = join[:cut + 1] + [f"{t + 1000} click 705 572"]         # Manager Status: Finish
    t += 90000                                                     # the host's Finish, the first news
    for step, name in (("click 45 165", "a-menu"), ("click 150 156", "b-squad"), ("click 169 527", "c-tactics")):
        join += [f"{t} {step}", f"{t + 3500} dump {{d}}/join1-{name}.bmp"]
        t += 5000
    x1, y1, x2, y2 = DRAG
    join += [f"{t} down {x1} {y1}", f"{t + 300} move {(x1 + x2) // 2} {(y1 + y2) // 2}", f"{t + 600} move {x2} {y2}",
             f"{t + 900} up {x2} {y2}", f"{t + 4000} dump {{d}}/join1-d-dragged.bmp",
             f"{t + 5000} click 705 573", f"{t + 9000} dump {{d}}/join1-e-closed.bmp",
             f"{t + 10000} click 169 527", f"{t + 14000} dump {{d}}/join1-f-again.bmp", f"{t + 16000} quit"]
    return host, join


def play(program, game, shots, host_display, join_display, slow):
    nr.DISPLAYS["host"], nr.DISPLAYS["join"] = host_display, join_display
    host, join = steps()
    work = tempfile.mkdtemp(prefix="cm0102-netdrag-")
    h, he = nr.start(program, game, work, "host", d.scaled(host, slow), shots, 1, {}, False)
    codes = []
    if nr.wait_for(os.path.join(shots, "host-stderr.txt"), f"screen {d.STATUS_BUILD}", h, 300 * slow):
        j, je = nr.start(program, game, work, "join1", d.scaled(join, slow), shots, 1, {}, False)
        codes.append(j.wait(timeout=600 * slow))
        je.close()
    else:
        codes.append("no Manager Status")
    codes.append(h.wait(timeout=600 * slow))
    he.close()
    return codes


def pitch_share(a, b, display):
    from PIL import Image, ImageChops
    box = PITCH[display]
    x, y = Image.open(a).convert("RGB").crop(box), Image.open(b).convert("RGB").crop(box)
    diff = ImageChops.difference(x, y)
    data = diff.get_flattened_data() if hasattr(diff, "get_flattened_data") else diff.getdata()
    return sum(1 for p in data if p != (0, 0, 0)) / (x.size[0] * x.size[1])


def main(argv):
    if len(argv) < 5 or "--out" not in argv:
        print(__doc__)
        return 2
    program, game = os.path.abspath(argv[1]), os.path.abspath(argv[2])
    out = os.path.abspath(argv[argv.index("--out") + 1])
    slow = float(argv[argv.index("--slow") + 1]) if "--slow" in argv else 1.0
    failed = []

    def check(ok, text):
        print(("ok   " if ok else "FAIL ") + text, flush=True)
        if not ok:
            failed.append(text)

    folders = {}
    for host_display in ("classic", "widescreen"):
        for join_display in ("classic", "widescreen"):
            shots = os.path.join(out, f"{host_display}-{join_display}")
            os.makedirs(shots, exist_ok=True)
            folders[(host_display, join_display)] = shots
            codes = play(program, game, shots, host_display, join_display, slow)
            check(codes == [0, 0], f"host {host_display}, joiner {join_display}: the games end with code 0 ({codes})")
    for join_display in ("classic", "widescreen"):
        a, b = folders[("classic", join_display)], folders[("widescreen", join_display)]
        dump = lambda folder, name: os.path.join(folder, f"join1-{name}.bmp")  # noqa: E731
        if not all(os.path.exists(dump(f, n)) for f in (a, b) for n in ("c-tactics", "d-dragged", "f-again")):
            check(False, f"joiner {join_display}: a dump is missing")
            continue
        for folder, host_display in ((a, "classic"), (b, "widescreen")):
            moved = pitch_share(dump(folder, "c-tactics"), dump(folder, "d-dragged"), join_display)
            check(moved > 0, f"joiner {join_display}, host {host_display}: the drag moved a player "
                             f"({moved:.2%} of the pitch changed)")
        for name in ("d-dragged", "f-again"):
            share = pitch_share(dump(a, name), dump(b, name), join_display)
            check(share == 0, f"joiner {join_display}, {name}: the pitch is the same under a classic and a widescreen "
                              f"host ({share:.2%} differs)")
    print(f"network drag run: {'PASS' if not failed else 'FAIL (' + str(len(failed)) + ')'}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))

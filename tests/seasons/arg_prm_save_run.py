#!/usr/bin/env python3
"""A save in the second half of a zone season of the Argentine Primera loads and completes the season (0.7.2 league
sizes, slice 15; spec docs/specs/2026-11-13-league-sizes-0.7.2-spec.md, slice 15 check 2).

Usage:
    python3 tests/seasons/arg_prm_save_run.py <cm0102_recomp> <game folder> [--keep <folder>] [--save-days n]
        [--play-days n] [--reuse]

The first program starts a 2025 game (the December 2025 database) with England and Argentina and changing rules on:
edition 2025 plays row 2024, the 2025 season of the rules table (Apertura and Clausura, each two zones of 15 with
the rival and drawn inter-zone rounds). It saves --save-days game days after the start (default 230: March 2026, in
the Clausura). A second program loads that save and plays --play-days (default 160) across the end of the season
and into edition 2026. The checks:
    first    the first run ends with code 0, no message box, the zone fixtures of both halves of 2025 in its log, and
             the save;
    load     the second run ends with code 0 and no message box and makes its save;
    played   at the start of edition 2026 (League::free_table 0x687970, RECOMP_SIZE_LOG) the table of 2025 holds 30
             clubs that each played 16 matches, so the Apertura table completed;
    next     edition 2026 makes its zones (row 2025) and both halves' fixtures after the load.
Exit code 1 on a failure. A run takes about an hour.
"""
import os
import re
import shutil
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
sys.path.insert(0, os.path.join(HERE, "..", "smoke"))
sys.path.insert(0, os.path.join(HERE, "..", "fixtures"))


def faults_first(work):
    log = open(os.path.join(work, "stderr.txt"), encoding="latin-1").read()
    fails = [x.strip() for x in log.split("\n") if "recomp: message box from" in x]
    for half in ("apertura", "clausura"):
        if not re.search(rf"recomp: league sizes: arg_prm fixtures 2025 {half}:( \|( \d+-\d+){{15}}){{16}}\s*$", log,
                         re.M):
            fails.append(f"first: no fixture line of 16 rounds of 15 matches for the {half} of 2025")
    if not os.path.exists(os.path.join(work, "user", "samesave.sav")):
        fails.append(f"first: no save ({work})")
    m = re.search(r"recomp: saved .* after (\d+) game days", log)
    print("first: " + (m.group(0) if m else "no save line"))
    return fails


def faults_load(again, code):
    log = open(os.path.join(again, "stderr.txt"), encoding="latin-1").read()
    fails = [x.strip() for x in log.split("\n") if "message box" in x]
    if code != 0:
        fails.append(f"load: the second run ended with code {code} ({again})")
    played = re.findall(r"recomp: league sizes: arg_prm played 2025: (\d+) clubs, (\d+) matches, played (\d+) to (\d+)",
                        log)
    if not played or played[-1] != ("30", "16", "16", "16"):
        fails.append(f"played: the 2025 table after the load: {played or 'no played line'}, want 30 clubs with 16 "
                     f"matches each")
    if not re.search(r"recomp: league sizes: arg_prm zones 2025: list 2025", log):
        fails.append("next: no zones of row 2025 (edition 2026) after the load")
    for half in ("apertura", "clausura"):
        if not re.search(rf"recomp: league sizes: arg_prm fixtures 2026 {half}:", log):
            fails.append(f"next: no {half} fixtures of 2026 after the load")
    return fails


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    import rules_report_check as rrc
    import club_world_check as cwc
    program, game = os.path.abspath(argv[0]), os.path.abspath(argv[1])
    keep = rrc.arg(argv, "--keep", None)
    save_days = int(rrc.arg(argv, "--save-days", "230"))
    play_days = int(rrc.arg(argv, "--play-days", "160"))
    work = os.path.abspath(keep) if keep else tempfile.mkdtemp(prefix="cm0102-arg-prm-save-")
    again = os.path.join(work, "again")
    os.environ["RECOMP_SIZE_LOG"] = "1"     # the load run inherits it (fixtures.run)
    if "--reuse" not in argv:
        db = rrc.run_database([], 2025, os.path.join(work, "owndb"))
        code, _ = rrc.run(program, game, 2026, 2025, db, ["England", "Argentina"], [], work, 7200,
                          {"changing_rules": "on"}, {"RECOMP_SIZE_LOG": "1", "RECOMP_SAVE_AFTER_DAYS": str(save_days)})
        if code != 0 and not os.path.exists(os.path.join(work, "user", "samesave.sav")):
            print(f"FAIL first: the run ended with code {code} and no save ({work})")
            return 1
    fails = faults_first(work)
    if not fails:
        if "--reuse" not in argv:
            os.makedirs(again, exist_ok=True)
            shutil.copyfile(os.path.join(work, "user", "samesave.sav"), os.path.join(again, "arg.sav"))
            r, saved = cwc.load_and_play(program, game, os.path.join(again, "arg.sav"), again, play_days, timeout=5400)
            code = r.returncode if saved else (r.returncode or 1)
        else:
            code = 0
        fails += faults_load(again, code)
    for f in fails:
        print("FAIL", f)
    print(("FAIL" if fails else "PASS") + f" arg_prm save in the Clausura of 2025 ({work})")
    return 1 if fails else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))

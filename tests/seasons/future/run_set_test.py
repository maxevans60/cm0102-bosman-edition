#!/usr/bin/env python3
"""ctest of tests/seasons/future/run_set.py without a game: the run set file, the legs of a row, the Tier B limits
and the release check (--verify) on made-up result folders.

Usage:
    python3 -I tests/seasons/future/run_set_test.py
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import run_set  # noqa: E402

GB = 1 << 30


def results(folder, head, rows):
    os.makedirs(folder, exist_ok=True)
    with open(os.path.join(folder, "results.tsv"), "w", encoding="utf-8") as f:
        f.write(head + "\n" + "\t".join(run_set.RESULT_HEAD) + "\n")
        for r in rows:
            f.write("\t".join(r) + "\n")


def main():
    bad = []
    rows = run_set.read_set()
    release = run_set.chosen(rows, "release")
    names = {r["name"] for r in release}
    for want in ("a2001s1", "a2001offs3", "a2025s3", "a2001esb", "a2001nation", "a2001to2300", "jump2298", "b2001"):
        if want not in names:
            bad.append(f"run set: {want} is not in the release set")
    if any(r["set"] == "quick" for r in release):
        bad.append("run set: a quick game is in the release set")
    if len([r for r in release if r["tier"] == "A" and r["last"] == 2100 and r["nations"] == "all"]) != 22:
        bad.append("run set: not 22 Tier A games of all nations to 2100 (7 starts, three seeds, and Q15)")

    legs = run_set.legs_of({"start": 2001, "last": 2100, "leg_years": 10})
    if len(legs) != 10 or legs[0] != (2001, 2010) or legs[-1] != (2091, 2100):
        bad.append(f"legs of 2001 to 2100 by 10: {legs[:2]} ... {legs[-1:]}")
    if run_set.legs_of({"start": 2098, "last": 2100, "leg_years": 1}) != [(2098, 2098), (2099, 2099), (2100, 2100)]:
        bad.append("legs of a date jump: not one season each")

    def leg(k, year, seconds, size, no_manager=frozenset()):
        return {"leg": k, "year": year, "seconds": seconds, "days": 365, "bytes": size, "no_manager": set(no_manager)}

    good = [leg(k, 2002 + k, 1000 + 10 * k, 200e6 + 1e6 * k) for k in range(30)]
    if run_set.tier_b_limits(good):
        bad.append(f"Tier B limits: faults in a good game: {run_set.tier_b_limits(good)}")
    checks = {
        "no-manager": good[:5] + [leg(5, 2007, 1050, 205e6, {"Wales"}), leg(6, 2008, 1060, 206e6, {"Wales"})],
        "slow": good[:20] + [leg(k, 2002 + k, 2500, 220e6) for k in range(20, 30)],
        "size": [leg(k, 2002 + k, 1000, 200e6 + 30e6 * k) for k in range(30)],
    }
    for check, game in checks.items():
        got = {c for c, _ in run_set.tier_b_limits(game)}
        if got != {check}:
            bad.append(f"Tier B limits: {check} not found alone, got {got}")
    one_season = good[:5] + [leg(5, 2007, 1050, 205e6, {"Wales"}), leg(6, 2008, 1060, 206e6, {"Chile"})]
    if run_set.tier_b_limits(one_season):
        bad.append("Tier B limits: a side with no manager in one save only fails")
    if [c for c, _ in run_set.tier_b_limits(good[:3] + [leg(3, 2005, 1000, 4 * GB)])] != ["size"]:
        bad.append("Tier B limits: a save of 4 GB is not a fault")

    # A passed leg: a kept leg keeps its save and the gzipped log, another leg the log only.
    work = tempfile.mkdtemp(prefix="run-set-")
    for keep in (True, False):
        leg_dir = os.path.join(work, f"leg-{keep}")
        for d in ("user/Databases/December 2025", "home", "shots"):
            os.makedirs(os.path.join(leg_dir, d))
        for n in ("user/" + run_set.SAVE_NAME, "user/match_stats_matches.tmp", "stderr.txt", "report.tsv"):
            open(os.path.join(leg_dir, n), "w").write("x")
        run_set.tidy(leg_dir, keep)
        left = sorted(os.path.relpath(os.path.join(d, n), leg_dir) for d, _, fs in os.walk(leg_dir) for n in fs)
        want = sorted(["report.tsv", "stderr.txt.gz"] + (["user/" + run_set.SAVE_NAME] if keep else []))
        if left != want:
            bad.append(f"tidy (keep {keep}): {left}, not {want}")

    # The release check on made-up runs: no save, so only the results and the commit count.
    head = run_set.git("rev-parse", "HEAD")
    passed = [[r["name"], r["tier"], r["set"], "pass", "", "2101", "10", "0", "1"] for r in release]
    results(os.path.join(work, "ok"), f"# commit {head} clean program x 2026-10-31 09:00", passed)
    if run_set.verify(os.path.join(work, "ok")):
        bad.append(f"verify: faults in a run where every game passed: {run_set.verify(os.path.join(work, 'ok'))[:3]}")
    failed = [r[:3] + (["fail", "3 checker faults"] if r[0] == "b2001" else r[3:5]) + r[5:] for r in passed]
    results(os.path.join(work, "fail"), f"# commit {head} clean program x 2026-10-31 09:00", failed)
    if not any(line.startswith("b2001: fail") for line in run_set.verify(os.path.join(work, "fail"))):
        bad.append("verify: a failed game passes")
    results(os.path.join(work, "short"), f"# commit {head} clean program x 2026-10-31 09:00", passed[1:])
    if not any("not in the run" in line for line in run_set.verify(os.path.join(work, "short"))):
        bad.append("verify: a game missing from the run passes")
    old = run_set.git("rev-list", "-1", "HEAD", "--", "src")          # the last commit that changed the game code
    results(os.path.join(work, "old"), f"# commit {old}~1 clean program x 2026-10-31 09:00", passed)
    if not any("game code changed" in line for line in run_set.verify(os.path.join(work, "old"))):
        bad.append("verify: a run of older game code passes")
    results(os.path.join(work, "dirty"), f"# commit {head} changes program x 2026-10-31 09:00", passed)
    if not any("changes to the game code" in line for line in run_set.verify(os.path.join(work, "dirty"))):
        bad.append("verify: a run from a changed checkout passes")
    code = subprocess.run([sys.executable, "-I", os.path.join(HERE, "run_set.py"), "--verify",
                           os.path.join(work, "fail")], capture_output=True).returncode
    if code != 1:
        bad.append(f"verify: exit code {code} for a failed run, not 1")

    for b in bad:
        print("FAIL  " + b)
    print(f"{'ok  ' if not bad else 'FAIL'}  run set: {len(release)} games in the release set, legs, Tier B limits and "
          f"the release check (work folder {work})")
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())

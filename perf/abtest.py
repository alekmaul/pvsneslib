#!/usr/bin/env python3
"""Test A/B de non-regression sur snes-examples : chaque projet est compile avec
deux chaines (A = reference, B = modifiee), lance N frames, et les ecrans compares.
Travaille sur une copie des exemples (--work) pour ne pas salir le depot.
Usage : abtest.py --work DIR [--opt-b 816-opt] [--cc-b 816-tcc] [--opt-a ...]
                  [--cc-a ...] [--frames 300] [filtre...]"""
import argparse
import filecmp
import os
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from runall import posix  # noqa: E402


def build(proj, env, overrides, full):
    if full:
        subprocess.run(["make", "clean"], cwd=proj, env=env, capture_output=True)
    else:
        # rebuild only the C code and the link: some converters (gfx4snes -M 5)
        # are not deterministic, the graphics of build A are reused
        for d, _, files in os.walk(proj):
            for f in files:
                base, ext = os.path.splitext(f)
                if ext == ".sfc" or (ext in (".obj", ".ps", ".asm") and base + ".c" in files):
                    os.remove(os.path.join(d, f))
    mk = subprocess.run(["make"] + overrides, cwd=proj, env=env, capture_output=True, text=True)
    roms = [f for f in os.listdir(proj) if f.endswith(".sfc")]
    if mk.returncode or not roms:
        return None, "\n".join((mk.stdout + mk.stderr).strip().splitlines()[-3:])
    return os.path.join(proj, roms[0]), ""


def screen(rom, frames, png):
    r = subprocess.run([sys.executable, os.path.join(HERE, "run_rom.py"), rom, str(frames), png],
                       capture_output=True, text=True)
    return r.returncode == 0


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--work", required=True)
    for side in "ab":
        ap.add_argument(f"--opt-{side}")
        ap.add_argument(f"--cc-{side}")
    ap.add_argument("--frames", type=int, default=300)
    ap.add_argument("filters", nargs="*")
    args = ap.parse_args()

    src = os.path.join(ROOT, "snes-examples")
    work = os.path.join(args.work, "snes-examples")
    if not os.path.isdir(work):
        # only the files tracked by git (the examples as published)
        tracked = subprocess.run(["git", "ls-files", "-z", "snes-examples"], cwd=ROOT,
                                 capture_output=True, text=True, check=True).stdout.split("\0")
        for rel in filter(None, tracked):
            dst = os.path.join(args.work, rel)
            os.makedirs(os.path.dirname(dst), exist_ok=True)
            shutil.copy2(os.path.join(ROOT, rel), dst)
    env = dict(os.environ, PVSNESLIB_HOME=posix(ROOT))

    def overrides(side):
        o = []
        for key, var in (("opt", "OPT"), ("cc", "CC")):
            v = getattr(args, f"{key}_{side}")
            if v:
                o.append(f"{var}=" + os.path.abspath(v).replace("\\", "/"))
        return o

    projects = []
    for d, _, files in os.walk(work):
        if "Makefile" in files and os.path.basename(d) != "res":
            with open(os.path.join(d, "Makefile"), encoding="latin-1") as f:
                if "snes_rules" in f.read():
                    projects.append(d)
    projects.sort()
    if args.filters:
        projects = [p for p in projects if any(f in p.replace("\\", "/") for f in args.filters)]

    stats = {"identique": 0, "ecran identique": 0, "ECRAN DIFFERENT": 0, "build": 0}
    tmp = tempfile.mkdtemp()
    for proj in projects:
        name = os.path.relpath(proj, work).replace("\\", "/")
        rom_a, err = build(proj, env, overrides("a"), True)
        if not rom_a:
            print(f"{name:55} build A impossible : {err}")
            stats["build"] += 1
            continue
        keep = os.path.join(tmp, "a.sfc")
        shutil.copy(rom_a, keep)
        rom_b, err = build(proj, env, overrides("b"), False)
        if not rom_b:
            print(f"{name:55} BUILD B EN ECHEC : {err}")
            stats["build"] += 1
            continue
        if filecmp.cmp(keep, rom_b, shallow=False):
            res = "identique"
        else:
            pa, pb = os.path.join(tmp, "a.png"), os.path.join(tmp, "b.png")
            if screen(keep, args.frames, pa) and screen(rom_b, args.frames, pb) \
                    and filecmp.cmp(pa, pb, shallow=False):
                res = "ecran identique"
            else:
                res = "ECRAN DIFFERENT"
                shutil.copy(pa, os.path.join(args.work, name.replace("/", "_") + "_A.png"))
                shutil.copy(pb, os.path.join(args.work, name.replace("/", "_") + "_B.png"))
        stats[res] += 1
        print(f"{name:55} {res}", flush=True)
        subprocess.run(["make", "clean"], cwd=proj, env=env, capture_output=True)
    print("---", ", ".join(f"{k}: {v}" for k, v in stats.items()))
    sys.exit(1 if stats["ECRAN DIFFERENT"] or stats["build"] else 0)


if __name__ == "__main__":
    main()

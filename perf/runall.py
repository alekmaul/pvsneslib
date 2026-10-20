#!/usr/bin/env python3
"""Reconstruit et lance tous les tests de perf/tests, puis affiche un resume.
Usage : runall.py [--opt 816-opt] [--cc 816-tcc] [--make "FASTROM=1"] [tests...]
Sans option, la chaine par defaut de snes_rules (devkitsnes) est utilisee."""
import argparse
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
FRAMES = {"bench": 900, "torture": 900, "divtest": 8000, "nmitest": 1800, "multest": 20000, "cmptest": 3000, "arrtest": 3000, "shifttest": 3000, "localtest": 3000, "sdivtest": 3000, "ptrtest": 3000, "bytetest": 3000, "cmultest": 3000, "pinctest": 3000, "vatest": 3000, "voltest": 3000, "structtest": 3000}


def posix(path):
    # snes_rules exige un PVSNESLIB_HOME au format /c/... (voir son en-tete)
    path = os.path.abspath(path).replace("\\", "/")
    m = re.match(r"^([A-Za-z]):/(.*)$", path)
    return f"/{m.group(1).lower()}/{m.group(2)}" if m else path


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--opt", help="816-opt a utiliser")
    ap.add_argument("--cc", help="816-tcc a utiliser")
    ap.add_argument("--make", default="", help="variables make en plus (ex. FASTROM=1)")
    ap.add_argument("tests", nargs="*", default=list(FRAMES))
    args = ap.parse_args()

    env = dict(os.environ, PVSNESLIB_HOME=posix(ROOT))
    overrides = args.make.split()
    if args.opt:
        overrides.append("OPT=" + os.path.abspath(args.opt).replace("\\", "/"))
    if args.cc:
        overrides.append("CC=" + os.path.abspath(args.cc).replace("\\", "/"))

    failed = False
    for test in args.tests:
        tdir = os.path.join(HERE, "tests", test)
        subprocess.run(["make", "clean"], cwd=tdir, env=env, capture_output=True)
        mk = subprocess.run(["make"] + overrides, cwd=tdir, env=env, capture_output=True, text=True)
        if mk.returncode:
            print(f"== {test} : ECHEC DU BUILD")
            print("\n".join((mk.stdout + mk.stderr).strip().splitlines()[-4:]))
            failed = True
            continue
        run = subprocess.run([sys.executable, os.path.join(HERE, "bench.py"),
                              os.path.join(tdir, test + ".sfc"), str(FRAMES.get(test, 1800))],
                             capture_output=True, text=True)
        lines = [l.strip() for l in run.stdout.splitlines() if l.strip()]
        if test == "bench":
            rows = [l for l in lines if re.match(r"^[A-Z0-9]+ +\d+ +-?\d+$", l)]
            total = sum(int(l.split()[1]) for l in rows)
            print(f"== bench : total {total} frames")
            print("   " + " | ".join(rows))
            ok = "DONE" in lines
        else:
            ok = any(l in ("OK", "END OK") for l in lines)
            errs = [l for l in lines if "ERR" in l]
            print(f"== {test} : {'OK' if ok else 'ECHEC'}  " + " | ".join(errs))
        if not ok:
            print("   " + " / ".join(lines))
            failed = True
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()

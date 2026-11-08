#!/usr/bin/env python3
"""Tests de 816-opt au niveau assembleur : chaque asmtests/NOM.ps est optimise,
et le code de sa fonction (du premier label au rtl) compare a asmtests/NOM.expected.
Usage : asmtest.py [--opt 816-opt]"""
import argparse
import glob
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)


def function_body(path):
    lines, on = [], False
    for l in open(path, encoding="latin-1"):
        l = l.strip()
        if not on and l.endswith(":") and not l.startswith(";"):
            on = True
        if on:
            lines.append(l)
            if l == "rtl":
                break
    return lines


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--opt", default=os.path.join(ROOT, "devkitsnes", "tools", "816-opt"))
    args = ap.parse_args()
    opt = os.path.abspath(args.opt)
    if sys.platform == "win32" and not opt.lower().endswith(".exe"):
        opt += ".exe"
    failed = 0
    for src in sorted(glob.glob(os.path.join(HERE, "asmtests", "*.ps"))):
        name = os.path.splitext(os.path.basename(src))[0]
        with tempfile.TemporaryDirectory() as tmp:
            out = os.path.join(tmp, "out.asm")
            subprocess.run([opt, "-q", "-i", src, "-o", out], check=True, capture_output=True)
            got = function_body(out)
        want = [l.strip() for l in open(os.path.splitext(src)[0] + ".expected", encoding="latin-1")
                if l.strip()]
        ok = got == want
        failed += not ok
        print(f"== {name} : {'OK' if ok else 'ECHEC'}")
        if not ok:
            print("   attendu : " + " / ".join(want))
            print("   obtenu  : " + " / ".join(got))
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()

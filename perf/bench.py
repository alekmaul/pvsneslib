#!/usr/bin/env python3
"""Lance une ROM de test (bench, torture...) avec run_rom.py et affiche le texte
de la console PVSnesLib lu dans la WRAM (plus besoin de lire la capture).
Usage : bench.py [rom.sfc] [frames]   (defaut : tests/bench/bench.sfc, 900 frames)"""
import os
import struct
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))


def symbol(sym_path, name):
    with open(sym_path, encoding="latin-1") as f:
        for line in f:
            parts = line.split()
            if len(parts) == 2 and parts[1] == name:
                return int(parts[0], 16)
    sys.exit(f"symbole {name} absent de {sym_path}")


def screen_text(wram, addr):
    # Carte 32x32 de mots ; tuile = code ASCII - 0x20 (police de la console).
    lines = []
    for row in range(32):
        words = struct.unpack_from("<32H", wram, addr + row * 64)
        lines.append("".join(chr((w & 0xFF) + 0x20) for w in words).rstrip())
    while lines and not lines[-1]:
        lines.pop()
    return lines


def main():
    rom = sys.argv[1] if len(sys.argv) > 1 else os.path.join(HERE, "tests", "bench", "bench.sfc")
    frames = sys.argv[2] if len(sys.argv) > 2 else "900"
    addr = symbol(os.path.splitext(rom)[0] + ".sym", "scr_txt_font_map") - 0x7E0000
    with tempfile.TemporaryDirectory() as tmp:
        png, wram_path = os.path.join(tmp, "s.png"), os.path.join(tmp, "wram.bin")
        subprocess.run([sys.executable, os.path.join(HERE, "run_rom.py"), rom, frames, png, wram_path],
                       check=True, stdout=subprocess.DEVNULL)
        wram = open(wram_path, "rb").read()
    print("\n".join(screen_text(wram, addr)))


main()

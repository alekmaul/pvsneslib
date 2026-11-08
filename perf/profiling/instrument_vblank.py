#!/usr/bin/env python3
"""Instrumente WaitForVBlank dans une COPIE de la bibliotheque PVSnesLib :
la ligne de balayage a l'entree est rangee dans perf_vline (RAM basse).
Usage : instrument_vblank.py chemin/vers/copie/pvsneslib/source/vblank.asm
Puis compiler la copie (make lib ; make HIROM=0 FASTROM=0 build) et lier le jeu
avec LIBDIRSOBJS=<copie>/pvsneslib/lib/LoROM_SlowROM."""
import sys

p = sys.argv[1]
s = open(p, encoding="latin-1").read()
a = "lag_frame_counter       dsb 2  ; Number of lag frames encountered (can be externally modified)\n"
b = """WaitForVBlank:
	php
	sep    #$20
.ACCU 8

	pha
"""
if "perf_vline" in s:
    sys.exit("deja instrumente")
assert a in s and b in s, "vblank.asm inattendu"
s = s.replace(a, a + "\nperf_vline              dsb 2  ; PERF: scanline when the main loop finished its frame\n", 1)
s = s.replace(b, b + """
	; PERF: scanline at which the main loop has finished its frame
	lda.l  $002137          ; latch the H/V counters
	lda.l  $00213F          ; reset the counters read flip-flop
	lda.l  $00213D          ; V counter, low byte
	sta.l  perf_vline
	lda.l  $00213D          ; V counter, bit 8
	and.b  #$01
	sta.l  perf_vline+1
""", 1)
open(p, "w", encoding="latin-1", newline="").write(s)
print("ok", p)

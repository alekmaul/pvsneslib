#!/usr/bin/env python3
"""Simulation de arrtest.c : affiche les #define EXP_* attendus (arithmetique 16 bits)."""
M = 0xFFFF

seed = 1234
def rnd():
    global seed
    seed = (seed * 25173 + 13849) & M
    return seed

b8 = [(i * 7) & 0xFF for i in range(300)]
w16 = [rnd() for _ in range(200)]
for i in range(199):
    for j in range(199 - i):
        if w16[j] > w16[j + 1]:
            w16[j], w16[j + 1] = w16[j + 1], w16[j]
for i in range(150):
    tb = b8[i]
    b8[i] = (b8[299 - i] + tb) & 0xFF
    b8[299 - i] = tb ^ 0x5A
for i in range(1, 200):
    w16[i] = (w16[i] + w16[i - 1] // 2) & M
tbl = [3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3]
grid = [[(r * c + tbl[(r + c) & 15]) & M for c in range(10)] for r in range(8)]
objs = [[i * 3, -i, i & 1] for i in range(20)]
for o in objs:
    if o[2]:
        o[0] += o[1]
s_w = sum(w16[i] ^ i for i in range(200)) & M
s_b = sum(b8) & M
s_g = sum(grid[r][c] * (c + 1) for r in range(8) for c in range(10)) & M
s_o = sum(o[0] * 3 + o[1] + o[2] for o in objs) & M
s_x = sum(tbl[i] * tbl[15 - i] for i in range(16)) & M
for k, v in (("W", s_w), ("B", s_b), ("G", s_g), ("O", s_o), ("X", s_x)):
    print(f"#define EXP_{k} {v}u")

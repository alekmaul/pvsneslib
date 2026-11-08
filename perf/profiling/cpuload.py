"""cpuload.py ROM... : charge CPU de rick1 sur le passage scripte.
Charge = lignes entre le debut du VBlank et la fin du travail d'une etape de jeu
(PAL : 312 lignes, VBlank a la ligne 240 en overscan)."""
import os, subprocess, sys, statistics, collections
R = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))  # racine du depot
LINES = 312
env = dict(os.environ,
           RUN_ROM_POKE="100:15470=8/2",
           RUN_ROM_INPUT="600-606:A,720-726:A,960-966:A,1100-1500:RIGHT",
           RUN_ROM_WATCH="1020-1500:v=37/2,lag=35/2,y=2eda/s2,scr=2e7e/1")
for rom in sys.argv[1:]:
    out = subprocess.run([sys.executable, os.path.join(R, "perf", "run_rom.py"), rom, "1501", os.devnull + ".png"],
                         env=env, capture_output=True, text=True).stdout
    rows = []
    for l in out.splitlines():
        if l[:1].isdigit() and ":" in l:
            f, rest = l.split(":", 1)
            d = dict(kv.split("=") for kv in rest.split())
            rows.append((int(f), int(d["v"]), int(d["lag"]), int(d["scr"])))
    vb = collections.Counter(v for _, v, _, _ in rows).most_common(1)[0][0]   # debut du VBlank
    work = lambda a, b: [(v - vb) % LINES for f, v, l, s in rows if a <= f < b and v != vb]
    walk = work(1100, 1270)
    scroll = [f for f, v, l, s in rows if s]
    lag = lambda a, b: [l for f, v, l, s in rows if a <= f < b]
    lw = lag(1100, 1270)
    print(f"{os.path.basename(rom):12} VBlank ligne {vb} | marche : charge mediane {statistics.median(walk):5.1f} (moy {statistics.mean(walk):5.1f}) "
          f"lignes (max {max(walk)}), lag {lw[-1] - lw[0]} | defilement : {len(scroll)} frames | "
          f"lag total {rows[-1][2] - rows[0][2]}")

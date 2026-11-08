"""profile.py ROM... : repartition de la charge d'une etape de jeu (marche) par phase."""
import os, subprocess, sys, statistics, collections
R = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))  # racine du depot
L = 312
w = ",".join([f"m{i}={0x1a1 + 2*i:x}/2" for i in range(4)] + [f"a{i}={0x1b1 + 2*i:x}/2" for i in range(8)]
             + ["v=37/2", "x=2ed8/s2"])
env = dict(os.environ, RUN_ROM_POKE="100:15470=8/2",
           RUN_ROM_INPUT="600-606:A,720-726:A,960-966:A,1100-1500:RIGHT",
           RUN_ROM_WATCH="1100-1270:" + w)
names = ["etape complete (process_controls -> update_score)", "draw_panel_all", "draw_entities", "update_score",
         "  dont anim Rick (gere_rick...)", "  dont anim autres entites", "  dont draw_sprite", "  dont aff_sprites_oam",
         "test_rick_col_map (cumul)", "test_col_vs_rick (cumul)", "test_col_bbox (cumul)", "test_nmi_col_map (cumul)"]
for rom in sys.argv[1:]:
    out = subprocess.run([sys.executable, os.path.join(R, "perf", "run_rom.py"), rom, "1271", os.devnull + ".png"],
                         env=env, capture_output=True, text=True).stdout
    rows = []
    for l in out.splitlines():
        if l[:1].isdigit() and ":" in l:
            d = dict(kv.split("=") for kv in l.split(":", 1)[1].split())
            rows.append({k: int(v) for k, v in d.items()})
    vb = collections.Counter(r["v"] for r in rows).most_common(1)[0][0]
    # une etape par changement de m0 (nouvelle etape de jeu)
    keys = [f"m{i}" for i in range(4)] + [f"a{i}" for i in range(8)]
    steps, prev = [], None
    for r in rows:
        k = tuple(r[x] for x in keys)
        if k != prev:
            steps.append(r); prev = k
    ph = collections.defaultdict(list)
    for r in steps:
        ph[0].append((r["m3"] - r["m0"]) % L)
        ph[1].append((r["m1"] - r["m0"]) % L)
        ph[2].append((r["m2"] - r["m1"]) % L)
        ph[3].append((r["m3"] - r["m2"]) % L)
        for i in range(8):
            ph[4 + i].append(r[f"a{i}"])
    print(f"== {os.path.basename(rom)} : {len(steps)} etapes (lignes, mediane)")
    for i, n in enumerate(names):
        print(f"   {n:40} {statistics.median(ph[i]):6.1f}")

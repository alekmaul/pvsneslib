# Profilage d'un jeu (charge CPU par frame et par fonction)

Outils utilisés pour mesurer rick1 (voir `perf/NOTES.md`, §6 et §8). Ils ne
modifient jamais le dépôt ni le jeu d'origine : on travaille sur des **copies**.

Principe : relever la **ligne de balayage** courante (compteur V du PPU, via
`$2137`, `$213F`, puis `$213D` lu deux fois) à des points choisis, la ranger en
RAM basse, et la lire à chaque frame avec `perf/run_rom.py` (`RUN_ROM_WATCH`).
Une durée en lignes vaut environ 170 cycles CPU par ligne en SlowROM.

## Fichiers

| Fichier | Rôle |
|---|---|
| `instrument_vblank.py` | Instrumente `WaitForVBlank` dans une copie de `pvsneslib/source/vblank.asm` : la ligne à laquelle le jeu a fini sa frame va dans `perf_vline`. |
| `perfmark.asm` | À copier dans `src/` d'une copie du jeu. `perf_mark(id)` (ligne courante dans `perf_marks[id]`), `perf_begin(id)` / `perf_end(id)` (durées cumulées dans `perf_acc[id]`, un chronomètre par identifiant, imbrications possibles), `perf_reset()` (remet les cumuls à zéro au début d'une étape de jeu). 8 identifiants. |
| `rick1_instrumentation.diff` | Exemple : les marques ajoutées dans `game.c` (boucle de jeu) et `entities.c` (`draw_entities`, fonctions de collision enveloppées). |
| `cpuload.py` | Charge par étape de jeu à partir de `perf_vline` (médiane pendant la marche, durée du défilement, frames en retard). |
| `profile.py` | Répartition d'une étape entre les marques et les cumuls de `perfmark.asm`. |

## Mode d'emploi (exemple rick1)

1. Copier le jeu hors du dépôt, et copier la bibliothèque à mesurer (par exemple `git archive HEAD pvsneslib | tar -x -C copie`).
2. Instrumenter la copie : `python instrument_vblank.py copie/pvsneslib/source/vblank.asm`, puis `make lib` et `make HIROM=0 FASTROM=0 build` dans `copie/pvsneslib`, depuis PowerShell ou cmd.
3. Dans la copie du jeu : copier `perfmark.asm` dans `src/`, ajouter les marques (voir le diff), et construire avec `make LIBDIRSOBJS=/c/.../copie/pvsneslib/lib/LoROM_SlowROM` (plus `OPT=` ou `CC=` pour comparer des chaînes).
4. Relever les adresses de `perf_vline`, `perf_marks` et `perf_acc` dans le `.sym`, puis les reporter dans `cpuload.py` et `profile.py`, qui contiennent celles de rick1 : `$37`, `$1A1`, `$1B1`.
5. `python cpuload.py rom1.sfc rom2.sfc` et `python profile.py rom1.sfc rom2.sfc`.

## Points d'attention

- **Scénario et adresses propres à rick1.** Les deux scripts contiennent le scénario joué et les adresses de rick1 : il faut les adapter pour un autre jeu.
- **Région.** Dans snes9x, rick1 tourne en PAL avec overscan : 312 lignes par image, VBlank à la ligne 240. Pour un jeu NTSC, c'est 262 lignes, et le VBlank commence à la ligne 225 (ou 240 en overscan). `cpuload.py` détecte le début du VBlank, mais le nombre de lignes (`LINES`) est fixé à 312.
- **Coût des marques.** Chaque marque coûte quelques dizaines de cycles : il faut comparer des builds instrumentés de la même façon.
- **SRAM vierge.** Avec une SRAM vierge, rick1 a besoin de `RUN_ROM_POKE="100:15470=8/2"`, sauf si son `strcpy` de `game_loadhighscores` a été corrigé.

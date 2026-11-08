# PVSnesLib : accélérer le code C (816-tcc) — notes de reprise

*État au 29 septembre 2026.* Piste retenue : **optimiser la chaîne 816-tcc** (§5 bis). Résultats mesurés au §8 : −48 % de temps sur le banc de test.

## 1. Objectif et contraintes

- Accélérer le code C de PVSnesLib. D'abord cherché en **remplaçant 816-tcc** par un autre compilateur (§2 à §5), puis en **optimisant 816-tcc et sa chaîne** (§5 bis, §8).
- **Garder WLA-DX** pour assembler et lier la ROM, ainsi que la bibliothèque PVSnesLib telle quelle (ses `.obj` ne sont pas recompilés).
- **Multi-banque obligatoire** : le code C d'un jeu ne tient pas dans une seule banque de 32 Ko.
- **Multiplateforme obligatoire** : Windows natif (sans WSL2), Linux et macOS. WLA-DX et les autres outils fonctionnent déjà partout ; seul le compilateur reste à trouver.

## 2. Ce qui a été réalisé (prototype « mos2wla », livré en `mos2wla.zip`)

Chaîne : `fichier.c → mos-clang (fork wbniv) → ELF → elf2wla.py → .asm WLA-DX → wla-65816 → .obj → wlalink`, avec la bibliothèque PVSnesLib inchangée.

| Fichier | Rôle |
|---|---|
| `snes_rules_mos` | Remplace la ligne `include .../snes_rules` du Makefile d'un projet. Inclut `snes_rules` et remplace la compilation C et l'édition de liens. Prépare `mosbuild/` une fois par projet. |
| `elf2wla.py` | Convertit un objet ELF llvm-mos en source WLA-DX lisible (désassemblage en commentaire). Il réémet les octets et transforme les relocations en expressions WLA (`<`, `>`, `:`, `.dw`, `.dl`). |
| `gen_glue.py` | Lit les prototypes PVSnesLib via l'AST JSON de clang. Génère la glue (170/171 fonctions) et un overlay des headers. |
| `build_runtime.py` | Recompile le bitcode de `libcrt.a`/`libc.a` du SDK llvm-mos et le convertit en objets WLA (135 objets). |
| `mos_select.py` | Ne lie que les objets runtime nécessaires, comme un linker d'archives. |
| `mos_check.py` | Vérification après l'édition de liens : signale les symboles utilisés en near mais placés hors banque 0 (oubli de `FAR`). |
| `runtime/mos_runtime.asm` | Registres imaginaires, init `.data`/`.bss`, pile logicielle, trampolines, appel ABI tcc (`mos_tcc_call`), division rapide, trampoline NMI. |
| `runtime/mos_glue_rt.c` | Réempilage des arguments printf au format tcc, `nmiSet`. |
| `runtime/mos_config.inc` | Banque du code, tailles des piles. |
| `tests/` | torture, bench, divtest, nmitest (`make` = llvm-mos, `make TCC=1` = tcc), plus `run_rom.py` (émulation headless via le cœur libretro snes9x, capture PNG et dump WRAM). |

### Validations (écrans comparés au pixel près avec la version tcc)

- `hello_world` : identique.
- Exemple `graphics/Backgrounds/Mode1` : identique, y compris avec les assets forcés en banque 3 (à condition de les déclarer `FAR`).
- `tests/torture` : identique. Couvre données initialisées, tables const, structures, arithmétique signée et 32 bits, récursion, pointeurs de fonction, switch, tableaux far, libc et printf.
- `tests/divtest` : 0 erreur sur 16 416 divisions.
- `tests/nmitest` : 0 erreur avec une callback VBlank en C qui appelle la lib pendant que le code principal l'appelle aussi.
- LoROM SlowROM et FastROM testés.

### Performances (frames, `tests/bench`, snes9x)

| Noyau | tcc | llvm-mos (mode 16 bits) | Gain |
|---|---:|---:|---:|
| Tri à bulles | 101 | 24 | ×4,2 |
| Physique (64 objets) | 55 | 16 | ×3,4 |
| Remplissage de tilemap | 59 | 9 | ×6,5 |
| Récursion (fib 18) | 26 | 17 | ×1,5 |
| Boucle de copie | 115 | 37 | ×3,1 |
| Division / modulo | 60 | 16 | ×3,75 |

Tous ces gains ont été obtenus **en mode 16 bits** (`+mos-a16 +mos-xy16`), qui n'existe que dans le fork wbniv. Division et modulo passent par des routines 65816 écrites à la main, qui utilisent le diviseur matériel SNES quand le diviseur tient sur 8 bits.

### Limite bloquante du prototype

**Tout le code C doit tenir dans une seule banque** (appels `JSR`/`RTS`). C'est inacceptable pour un vrai jeu : c'est ce qui a relancé la recherche de compilateur.

## 3. Faits techniques établis

### Convention d'appel de 816-tcc (celle de la lib PVSnesLib)

- Arguments empilés de droite à gauche, à leur taille naturelle : `u8` sur 1 octet, `u16` sur 2, pointeur sur 4 (`[lo][hi][banque][0]`).
- L'appelant nettoie la pile ; appel par `JSL`, retour par `RTL`.
- Valeur de retour dans `tcc__r0` et `tcc__r0h`.
- CPU en 16 bits (A/X/Y), DBR = `$7E`, D = 0. Les registres tcc sont en page directe `$00-$2F`.
- Dans tcc, `int` et `long` font **2 octets**, `long long` 4. PVSnesLib définit `u32` comme `unsigned long long`.
- `crt0` appelle `main` par `jsr.l` avec A/X/Y en 16 bits.
- La chaîne actuelle de `snes_rules` : 816-tcc → 816-opt → wla-65816 (constify n'est plus utilisé).

### Fork llvm-mos-65816 (wbniv)

- ABI aux appels : M=8, X=8. Arguments dans A, X, puis `__rc2`… ; pointeurs de fonction dans des paires `__rc`.
- Un pointeur far vaut `[lo][hi][banque][0]`, comme chez tcc.
- Un cast near → far met la banque à 0. **Il faut donc DBR = `$00`** pendant le code C ; les données near sont en RAM basse ou en ROM banque 0.
- `jmp (abs,X)` (tables de sauts des `switch`) lit sa table dans la banque du **code** (PBR) et non dans DBR.
- Les variables locales des fonctions non récursives vont dans une « pile statique » non réentrante (`.noinit..Lstatic_stack`).
- Le far est opt-in : une fonction dans une section `.far_*` revient par `RTL` et le compilateur l'appelle par `JSL`.
- Via `#pragma clang attribute push(__attribute__((section(".far_text"))), apply_to=function)`, **tous les appels entre fonctions deviennent `JSL` avec relocation 24 bits**, même vers une fonction `static` de la même section, et les appels terminaux deviennent `JML`.
- En revanche, les **libcalls** générés par le compilateur (`__udivhi3`, `memcpy`, `__call_indir`…) **restent des `JSR`**, et les pointeurs de fonction restent sur 16 bits (la banque est calculée puis jetée).
- Un `#pragma clang attribute push` non refermé est une erreur : il faut un fichier enveloppe (`push` / `#include "fichier.c"` / `pop`).

### Pièges WLA-DX

- Un label commençant par `_` est local à sa section : on préfixe par `mos` (`__rc0` → `mos__rc0`).
- wlalink supprime les sections non référencées, et tolère les références non résolues dans les sections supprimées.
- `APPENDTO` vers une section `KEEP` garde tout ce qui y est ajouté, y compris les `.data` de modules inutilisés. D'où `mos_select.py`.
- wlalink est lancé avec `-c`, qui accepte les doublons en silence. Il faut donc renommer côté llvm-mos les symboles de libc (`memcpy`, `strlen`, `malloc`…) en `mos_*`.
- Syntaxe `mvn src,dst`.

### Règles de portage établies

- Les assets et données définis en asm doivent être déclarés `FAR` dans le C (`extern char FAR tiles;`).
- L'overlay des headers passe les 54 variables de la lib en far, les entiers en `volatile` (modifiés sous interruption) et les 3 pointeurs membres de structure en far. Il corrige aussi `u32`/`s32` en `unsigned long`.
- printf : `%ll` lit un `u32` ; `%l` reste tronqué à 16 bits comme avec tcc.
- Avec `nmiSet`, il faut appeler `consoleVblank()` dans la callback si l'on utilise `consoleDrawText`. `consoleVblank` existe dans la lib mais n'est déclarée dans aucun header.

### Bugs trouvés

- **816-tcc** ne promeut pas un `u8` passé en argument variadique : un octet de déchet s'affiche en plus (6161 au lieu de 17).
- **llvm-mos-65816 (wbniv)** : comparer un pointeur far à `NULL` fait planter le compilateur (`unable to legalize ... G_CONSTANT`). En mode 8 bits, les pointeurs far plantent aussi (`G_UNMERGE_VALUES`, `G_MERGE_VALUES`).
- **Archive `libc.a` du SDK** : les membres `malloc.cc.obj` et `malloc.s.obj` ont le même nom de base, et `setjmp.S.obj` apparaît en double. Il faut les extraire un par un (`llvm-ar xN`).

## 4. Piste multi-banque avec le fork wbniv (conçue, non implémentée)

1. Compiler chaque `.c` via une enveloppe avec le `#pragma` `.far_text` : toutes les fonctions deviennent far (`JSL`/`RTL`).
2. Faire découper par `elf2wla` la section de code **par fonction** (symboles `STT_FUNC`), chaque morceau dans une section WLA `SUPERFREE`. Vérifier qu'aucun `jmp` 16 bits ne sort de sa fonction.
3. Fusionner les tables de sauts (`jmp (abs,X)`) dans le morceau de la fonction qui les utilise, puisqu'elles doivent être dans la même banque.
4. Faire passer les libcalls (`JSR` depuis n'importe quelle banque) par des **relais en RAM basse** (`$0000-$1FFF`, visible dans toutes les banques LoROM) : `jsl thunk ; rts`, puis un thunk dans la banque du runtime : `jsr impl ; rtl`. Les relais sont copiés au démarrage comme des `.data`.
5. Faire pointer les pointeurs de fonction (valeurs 16 bits) vers des relais RAM `jsl f ; rts` pour chaque fonction dont l'adresse est prise. Il faut connaître l'ensemble des fonctions au moment du link, d'où un pilote de link (`mos_link.py`) qui convertit les objets avec cette connaissance globale.
6. Déclarer les fonctions de glue en sections far distinctes, faire revenir `mos_tcc_call` par `RTL` et remplacer le `jsr mos_main` du runtime par un `jsl`.
7. Inclure les headers standards (`string.h`…) **avant** le `push`, pour que les appels libc restent near.

Observation au dernier test : une copie de structure et un `strcpy` dans du code far ont donné des `JSR memcpy`, qui passeraient donc par les relais RAM.

## 5. Compilateurs évalués

| Candidat | Multi-banque | Code 16 bits | Windows / macOS / Linux | Verdict |
|---|---|---|---|---|
| **llvm-mos-65816 (wbniv)**, <https://github.com/wbniv/llvm-mos-65816> | Possible avec la piste du §4 | Oui (`+mos-a16`) | Linux x86-64 publié ; Windows compilé mais non vérifié ; macOS reporté. Vise l'intégration dans llvm-mos officiel, qui publie pour les 3 OS. | Base actuelle du prototype. |
| **llvm816 (Duensing)**, <https://forge.duensing.digital/AI_Slop/65816-llvm-mos/> | **Natif** : tout est `JSL`/`RTL`, linker multi-segments, pointeurs de fonction via `__jsl_indir` | Oui, invariant M=16/X=16 | **Aucun binaire** ; build Ubuntu (30-60 min, 16 Go de RAM, ~20 Go de disque). Cible W65816 séparée, non destinée à llvm-mos. | Meilleur candidat technique, mais il faudrait produire soi-même les binaires 3 OS (CI). Code généré par IA, un seul développeur. |
| **SDK gingerbeardman**, <https://github.com/gingerbeardman/llvm-mos-sdk-snes> | Non : placement manuel, `JSR` uniquement | Non | Oui, via les binaires llvm-mos officiels | Ce n'est pas un compilateur : un brouillon de plateforme SNES (un commit, oct. 2025) sur le llvm-mos officiel. Écarté. |
| **cc65**, <https://cc65.github.io/> | Non | Non : le compilateur C ne produit que du 6502/65C02 (seuls ca65 et ld65 gèrent le 65816) | Oui | Écarté : fait moins bien que le llvm-mos officiel, lui aussi multiplateforme. |

Déjà écartés pendant les recherches : jeremysrand/llvm-65816 (arrêté), WorldsApartDevTeam/65816-c (très précoce), Calypsi (propriétaire, gratuit seulement en usage amateur, format objet non compatible WLA).

### Points à vérifier pour llvm816 (non tranchés par sa documentation)

1. Le déréférencement de pointeur mentionne « l'octet de banque en `$E2` forcé à 0 ». Si tous les pointeurs visent la banque 0, passer des assets par pointeur à PVSnesLib serait impossible.
2. Les globales sont accédées en absolu relatif à DBR : les tables `const` en ROM posent le même problème que chez wbniv.
3. Les registres imaginaires occupent des emplacements fixes en page directe (`$C0-$FF`) : il faudra les réserver face aux variables de PVSnesLib.

## 5 bis. Piste « optimiser 816-tcc » (évaluée en dernier)

Dépôt : <https://github.com/alekmaul/tcc> (générateur 65816 : `816-gen.c`, sortie WLA dans `tccelf.c`).
Optimiseur actuel : `tools/816-opt` dans PVSnesLib. C'est un optimiseur à trous de serrure d'environ 1 100 lignes (`optimizer.c`), qui applique des motifs par expressions régulières, ligne par ligne, sans graphe de flot ni analyse de durée de vie. Usage : `816-opt -i f.ps -o f.asp`.

**Avantage décisif :** cette piste garde tout. tcc est déjà multiplateforme, le multi-banque existe déjà (fonctions en `SUPERFREE`, appels `JSL`), il n'y a ni changement d'ABI, ni glue, ni `FAR`, et les projets existants se recompilent sans modification.

Défauts observés dans le code tcc (boucle interne du tri à bulles, après 816-opt) :

- aucune allocation de registres : les variables locales font l'aller-retour avec la pile (`lda n,s` / `sta n,s`) et tout transite par `tcc__r0` ;
- une comparaison fabrique d'abord un booléen dans X (`ldx #1 / sbc / bcc / dex / stx tcc__r5 / txa / bne / brl / bra`), soit environ 12 instructions au lieu de 2 ou 3 ;
- chaque `arr[j]` reconstruit un pointeur 24 bits dans `tcc__r1`/`tcc__r1h`, puis lit par `[tcc__r1]`, au lieu d'un `lda.l arr,x` (4 fois par itération) ;
- l'incrément des boucles `for` est placé hors de la boucle, avec des sauts en chaîne (`jmp` → `bra` → `brl`) ;
- `tcc__mul` et `tcc__udiv` (`libtcc.asm`) sont entièrement logiciels : le multiplicateur et le diviseur matériels SNES ne sont pas utilisés.

| Niveau | Contenu | Effort | Gain estimé | Risque |
|---|---|---|---|---|
| 1. Runtime | Multiplication et division matérielles dans `libtcc.asm` | Quelques jours | Fort sur l'arithmétique (MATH : 60 → vers 16 frames ?) | Très faible |
| 2. 816-opt enrichi | Fusion comparaison + branchement, suppression des chaînes de sauts, des allers-retours inutiles par `tcc__r0` | 1 à 3 semaines | ×1,2 à ×1,5 sur les boucles | Faible |
| 3. Optimiseur avec analyse de flot | Graphe de flot et durée de vie des `tcc__r*` : `lda.l arr,x`, élimination des calculs d'adresse répétés, compteur de boucle en X/Y si la boucle n'appelle rien | 1 à 3 mois | ×2 environ, cumulé | Moyen |
| 4. Corriger `816-gen.c` | Comparaisons branchées sur les drapeaux, adressage indexé des tableaux globaux, opérandes lus dans la pile (`adc n,s`) | Quelques semaines à 2 mois | Semblable au niveau 3, plus robuste | Moyen |

Les gains sont **estimés**, pas mesurés. Plafond : tcc compile en une passe, sans représentation intermédiaire, donc sans allocation de registres sur la fonction entière, sans inlining ni optimisation de boucle. Il resterait probablement environ deux fois plus lent que llvm-mos en 16 bits.

L'outillage de test de `mos2wla` (émulateur sans interface, comparaison d'écrans, tests torture/bench/divtest, `make TCC=1`) resservirait tel quel pour valider chaque optimisation. On en profiterait pour corriger le bug de tcc sur les `u8` variadiques.

**Recommandation :** commencer par le niveau 1 et la fusion comparaison + branchement, mesurer avec `tests/bench`, puis décider. Optimiser tcc peut servir de solution d'attente pendant qu'on surveille l'intégration d'un fork 65816 dans le llvm-mos officiel.

**Décision (28 septembre 2026) : piste retenue.** Les niveaux 1 à 3 sont faits et mesurés (§8). Les gains réels dépassent les estimations ci-dessus : −11 % au niveau 1, −19 % cumulés au niveau 2, −48 % cumulés au niveau 3. Niveau 4 commencé le 29 septembre : division et modulo signés par une puissance de 2 constante, générés en ligne au lieu d'appeler `tcc__div` (`816-gen.c`, testés par `sdivtest`). Effet sur rick1 : −2 % par étape (un `x/8` par appel de `test_rick_col_map`). Puis multiplication par une constante ayant 2 ou 3 bits à 1 (tailles de structures : 6, 10, 12, 20, 24, 72…) calculée en ligne par décalages et additions au lieu d'appeler `tcc__mul` (`cmultest`) : rick1 −8 % par étape (`&tab_entities[i]`, 72 octets), banc 215 → 202 frames. Puis coût des appels (1er octobre) : banc 202 → 196 frames, FIB18 21 → 15, sans effet mesurable sur rick1 (90 lignes par étape), dont le temps ne part pas dans les appels :
  - `tccgen.c` : un registre entier sauvegardé sur la pile (`save_reg`) n'occupe plus que 2 octets au lieu de 4. Il est relu avec son propre type, donc le mot de banque n'était jamais relu ;
  - `816-gen.c` : le `return` d'une fonction qui renvoie un entier ne recopie plus le mot de banque (`tcc__r0h`). Cette réduction ne vaut **que** pour le `return` : ailleurs, tcc retype en `int` des valeurs qui sont des pointeurs (copie de l'ancienne valeur dans `*p++`, `gv_dup`). Une première version qui réduisait toutes les copies typées entier cassait rick1 (carte illisible) sans qu'aucun test ni abtest ne le voie ; `pinctest` (pointeurs incrémentés vers deux banques en alternance) le détecte désormais (36 erreurs) ;
  - 816-opt : suppression d'un saut inconditionnel vers l'étiquette qui le suit, et durée de vie des registres de retour des fonctions `static` calculée comme plus petit point fixe (une fonction récursive gardait sinon ses propres registres de retour vivants).

  Puis (1er octobre) banc 196 → 176 frames (151 en FastROM), SORT 65 → 51, MAP 35 → 29, rick1 inchangé :
  - **`u8` variadiques corrigés** (`tccgen.c`, `gfunc_param_typed`) : un `char` ou un `bool` passé dans `...` est promu en `int` (2 octets), ce que lit `va_arg(ap, int)` dans la bibliothèque (`%d`, `%u`, `%c`). COPY affiche enfin 17. Test `vatest` (13 erreurs avant, 0 après). Aucun écran d'exemple ne change ;
  - **opérandes lus directement** (`flow.c`, passe des valeurs) : `adc/sbc/cmp/and/ora/eor` sur un pseudo-registre qui contient une constante lit `#c` ; s'il contient une case de pile et qu'il est mort ensuite, l'opération lit `n,s` et la copie devient du code mort. Exemple : la borne `99 - i` de SORT passe de 11 à 7 instructions ;
  - **calculs sur A** (`flow.c`) : une suite `inc/dec/asl/lsr/rol/ror` faite en mémoire sur un pseudo-registre qui vaut déjà A, puis relu par `lda` (ou par `ldx`/`ldy` si A est mort ensuite), se fait sur A : 2 cycles par opération au lieu de 7 ;
  - **bug latent corrigé dans 816-opt** : deux anciennes règles du peephole (`sta.b rX / asl.b rX` → `asl a / sta.b rX`, et `adc #c / sta rX / inc rX / inc rX` → `adc #c + 2`) changent la valeur de A en supposant qu'elle n'est plus lue. C'est vrai sur la sortie de tcc, qui recharge toujours A, mais plus après l'analyse de flot, qui supprime ces rechargements. Elles ne s'appliquent plus que dans la passe qui précède l'analyse. Trouvé par un test assembleur, jamais observé sur du vrai code.

  Tests assembleur ajoutés : `operand_ok`, `operand_push` (pas de lecture de `n,s` après un `pha`), `rmw_chain`, `rmw_index`.

  Puis lectures d'octets (1er octobre) : banc 176 → 173 frames, rick1 90 → 87 lignes par étape (−3 %) :
  - **816-opt** (`byteRead`, dans la passe qui suit l'analyse de flot) : `lda.w #0 / [ldy #c /] sep #$20 / lda X / rep #$20` devient `[ldy #c /] lda X / and.w #$00FF`, soit 9 cycles au lieu de 14. Cela vaut pour une globale, une case de pile ou un pointeur. 432 des quelque 520 lectures d'octets de rick1 sont converties. Ni à une adresse numérique, ni avant un branchement (le drapeau N diffère) ;
  - la lecture sur 16 bits lit aussi l'octet suivant, ce qui est sans effet en mémoire mais pas sur un registre matériel : lire `$2180` avance l'adresse WRAM, lire `$4210` efface le drapeau de NMI. **tcc** (`816-gen.c`, `VOLATILE_BYTE`) lit donc un octet `volatile` sous une autre forme (`sep / lda / rep / and.w #$00FF`), que 816-opt ne touche pas. Pour cela, `gv()` (`tccgen.c`) garde désormais le qualificatif `volatile` dans le type de l'accès mémoire, qu'il perdait ;
  - **les deux outils vont ensemble** : ce 816-opt avec un ancien 816-tcc élargirait aussi les lectures `volatile` faites par pointeur ;
  - test `voltest` : lit `$217F` par un pointeur `volatile`, puis vérifie que l'adresse du port `$2180` n'a pas avancé (2 erreurs si tcc ne marque pas le `volatile`) ; test assembleur `byte_read`.

  Puis motifs de boucles de jeu (19 octobre), repérés dans **chipschallenge** (jeu sur grille, boucles à compteurs `u8` globaux et tableaux de structures d'objets) : rick1 87 → 82 lignes par étape (−6 %) ; chipschallenge −5 % de code (36 010 → 34 135 lignes d'assembleur, 84 → 228 accès indexés) ; banc inchangé (173 frames, ses noyaux sont des fonctions `static` sans compteur `u8`) :
  - **type de retour déclaré** (`816-gen.c` + `flow.c`) : tcc écrit `.define __f_ret N` avant chaque fonction (0 `void`, 1 entier dans r0, 2 pointeur r0/r0h, 3 `long long`, 4 flottant, 5 inconnu). 816-opt ne garde vivants à la sortie que ces registres, au lieu de r0 à f0h, pour **toutes** les fonctions et pas seulement les `static`. C'était la limite qui bloquait le plus de réécritures (un pointeur temporaire restait « vivant » jusqu'au `rtl`) ;
  - **comparaison d'un `u8` à une constante** (`unsignedCompares`) : `sec / sbc #c / [beq] / bvc + / eor #$8000 / + / bmi|bpl T` devient `cmp #c / [beq] / bcc|bcs T` quand A est prouvé dans 0..`$7FFF` (octet lu) et `c` aussi : ordres signé et non signé identiques. Pas pour un `s8` ;
  - **`++`/`--` d'un `u8`** (`byteIncrements`) : `lda #0 / sep / lda V / rep / sta rT / inc rT / sep / lda rT / sta V / rep` devient `sep / inc V / rep` (ou `lda / inc a / sta` en 8 bits pour une case de pile ou une adresse longue), si A et rT sont morts ensuite ;
  - **champ d'un tableau de structures** (`indexArrays`) : le décalage du champ (`inc a` ou `clc / adc #C`) entre l'indice et le pointeur passe dans l'accès, `lda.l SYM + C,x`, avec la même limite qu'avant pour un indice négatif ;
  - **les deux outils vont ensemble** : un ancien 816-opt refuse d'analyser une fonction qui contient la ligne `.define` (directive inconnue) et perd alors toutes les optimisations de flot ; ce 816-opt avec un ancien tcc garde l'hypothèse prudente (tous les registres de retour) ;
  - test `structtest` (25 vérifications : rebouclage 255 → 0, comparaisons aux bornes, `s8`, champs, retours `u8`/`u16`/pointeur ROM et RAM/`long long`/`void`). Sensibilité vérifiée : sans la preuve 0..`$7FFF`, 1 erreur ; sans la banque du pointeur renvoyé, 3 erreurs. Tests assembleur `ucmp`, `byte_inc` ;
  - chipschallenge construit dans une copie : écrans identiques à la chaîne installée jusqu'au titre (au-delà, le jeu reste sur un écran violet dans le snes9x sans affichage, y compris la ROM d'origine de l'auteur). rick1 : écrans identiques frame à frame jusqu'au défilement ;
  - `byteRead` regarde aussi au-delà d'un épilogue (`.ifgr`/`.endif`) et d'un saut inconditionnel vers une étiquette nommée, pour trouver l'instruction qui lirait N. Sur les 84 fichiers C des exemples : 32 448 → 31 151 instructions (−4 %), 35 fichiers plus petits, aucun plus gros ;
  - abtest : aucun écran différent, sauf le logo Konami à 300 frames, identique à 600 et 900 frames (décalage dans le temps).

  **Vitesse sur trois jeux (19 octobre)**, même bibliothèque instrumentée (`WaitForVBlank` relève la ligne de balayage) pour les trois chaînes ; « origine » = `816-tcc.orig` + `816-opt.orig`, donc gain du compilateur seul, hors niveau 1 :

  | Jeu, passage | Mesure | Origine | Chaîne du 1er oct. | Chaîne du 19 oct. | Gain total |
  |---|---|---:|---:|---:|---:|
  | rick1, marche + puits (PAL) | lignes par étape de jeu | 124 | 87 | 82 | −34 % |
  | rick1 | charge médiane par frame | 128 | 88 | 84 | −34 % |
  | rick1 | frames en retard | 35 | 24 | 23 | −34 % |
  | geeklife, niveau 1 en marchant (NTSC) | charge médiane par frame | 118 | 115 | 114 | −3 % |
  | geeklife, intro mode 7 | charge médiane par frame | 10 | 9 | 9 | — |
  | chipschallenge, intro | charge médiane par frame | 10 | 10 | 10 | — |
  | chipschallenge, niveau 1, Chip qui se déplace (NTSC, FastROM, après correction du jeu) | charge médiane (moyenne) par frame | 70 (73,2) | 62 (65,1) | 61 (64,0) | −13 % |

  - geeklife suit exactement le même déroulé avec les trois chaînes (écran identique à la frame 4500). Sa charge part surtout dans la bibliothèque en assembleur (moteur de sprites, OAM, DMA), que le compilateur ne touche pas ;
  - chipschallenge : l'écran violet après Start venait d'un bug du jeu, pas de l'émulateur. La routine VBlank (`vlanbks.asm`) lisait ses drapeaux (`dorefresh`…, `num_vbl`), placés en banque `$7F`, en adressage absolu avec DBR = `$7E` : elle lisait `metatiles` à `$7E:8000` et ne transférait jamais le texte. Corrigé par l'auteur le 20 octobre ; le jeu est alors mesurable (scénario : Start 2300, 3000, 3300, 3600, A 3900, Start 4300, puis Chip se déplace de 4500 à 5000). Écran identique à la frame 5000 avec les trois chaînes. Code : −5 % d'instructions, 2,7 fois plus d'accès indexés ;
  - rappel pour le code assembleur des jeux : le C compilé par tcc + 816-opt suppose **DBR = `$7E`** (816-opt écrit en `.w` les accès aux globales C de la section `.bss`). Une routine qui change de banque doit la remettre à `$7E` avant d'appeler une fonction C.

## 6. Prochaines étapes possibles

### Sur la piste retenue (optimiser 816-tcc)

1. **Consolider** : compiler 816-opt sous Linux et macOS (seul Windows a été fait). Vérifier dans l'environnement officiel de build Windows (MSYS2 ?) si le `-liconv` du Makefile de 816-opt peut être retiré : il est inutile avec w64devkit, où il faut redéfinir `LDFLAGS`.
2. **Mesurer sur un vrai jeu** pour savoir où part le temps CPU hors du banc synthétique : appels de fonction, pointeurs, boucles. Candidat retenu : **rick1** (Rick Dangerous, `C:\svgexterne\vboxshared\DropboxSvnClient\snes\rick1`).
   - Idée : jouer un passage scripté (premier écran, chute dans le puits, rocher) avec `run_rom.py` (`RUN_ROM_INPUT`, `RUN_ROM_WATCH`), et compter les frames avec la chaîne d'origine puis avec la chaîne actuelle.
   - Scénario qui fonctionne (LoROM SlowROM) : A aux frames 600, 720 et 960 (titre, choix du niveau, histoire), puis RIGHT de 1100 à 1500. Rick tombe dans le puits vers la frame 1290 et le défilement commence vers 1310.
   - Variables utiles : Rick = `tab_entities[1]`, x à `$2ED8`, y à `$2EDA` ; `flag_scroll` à `$2E7E` ; `offset_map_y` à `$154B8` (banque `$7F`). Ces adresses changent à chaque build : les relire dans `rickdangerous1.sym`.
   - Avec une SRAM vierge, il faut `RUN_ROM_POKE="100:15470=8/2"` : un `strcpy` de `game_loadhighscores` déborde sur `world_start_crds` (voir le bug signalé à l'auteur), sinon le jeu saute le niveau 1.
   - Toujours construire rick1 dans une copie (le dossier du jeu n'est pas versionné).
3. **Selon la mesure** :
   - niveau 4 (`816-gen.c`) : compteurs de boucle dans X ou Y, incrément de boucle sans sauts en chaîne, appels moins coûteux (FIB18 n'a gagné que 26 → 21), promotion des `u8` variadiques ;
   - ou poursuivre le niveau 3 : pointeurs avec décalage non nul, `inc a / inc a` sur un pointeur, comparaisons 32 bits.

### Pistes « autre compilateur » (historique, mises de côté)

1. **Mesurer le llvm-mos officiel en 8 bits face à tcc.** C'est la seule piste qui règle le multiplateforme sans compiler LLVM soi-même. La mesure a été tentée avec le toolchain wbniv sans `+mos-a16` : la glue actuelle (pointeurs far) ne compile pas en 8 bits, et la mesure n'a pas abouti. Il faudrait une glue sans type far (pointeurs passés en octets, banque 0 pour le near) et un mécanisme maison pour l'adresse 24 bits des assets. Si le gain face à tcc est net, implémenter la piste du §4 sur le compilateur officiel.
2. **Ou évaluer llvm816 :** le compiler (plutôt sur une machine multicœur ; l'environnement de Claude n'a qu'un cœur), puis passer les tests existants et l'exemple Mode1 (assets en banque 3) pour trancher les trois points du §5. Adapter `elf2wla` à ses types de relocation.
3. **Ou poursuivre avec wbniv :** implémenter le §4 et suivre l'intégration de ses correctifs dans llvm-mos officiel, pour les binaires 3 OS.
## 7. Environnement de test utilisé (pour reproduire)

### Prototype llvm-mos (Linux)

- PVSnesLib compilé depuis les sources : 816-tcc (dépôt alekmaul/tcc), 816-opt (`tools/816-opt`), WLA-DX (vhelin/wla-dx, compilé avec cmake), puis bibliothèque via `make` et `make FASTROM=1` dans `pvsneslib/source`.
- Toolchain wbniv : `llvm-mos-65816-20260914-f9711be-linux-x86_64.tar.xz` (release `toolchain-20260914-f9711be` du dépôt `wbniv/indri.studio`).
- Émulation headless : paquet Ubuntu `libretro-snes9x` + `tests/run_rom.py` (frontend libretro en Python/ctypes).
- `gfx4snes` compilé depuis `tools/gfx4snes` pour les exemples graphiques.

### Piste 816-tcc (Windows natif)

- **Compilateur C hôte** : w64devkit v2.10.0 (gcc 16.2, make 4.4.1, cmake), installation portable dans `C:\w64devkit`. pcre2 10.49 compilé en statique et installé dans w64devkit pour 816-opt.
- **816-tcc** : `./configure && make` dans `compiler/tcc`. Le code produit est identique au binaire fourni ; seuls diffèrent des commentaires de débogage contenant des adresses.
- **816-opt** : `make LDFLAGS='-lpthread -static -lpcre2-posix -lpcre2-8'` dans `tools/816-opt`. Le `-liconv` du Makefile est inutile, le code ne s'en sert pas.
- **WLA-DX** : cmake hors du sous-module (`-G "MinGW Makefiles"`), cibles `wla-65816 wla-spc700 wlalink`. Les ROMs produites sont identiques octet pour octet à celles des binaires fournis.
- **Builds des ROMs** : le make de msys (`ndsdev/msys`), lancé depuis PowerShell ou cmd. Depuis Git Bash, `PVSNESLIB_HOME` est converti en `C:/...` et la création du linkfile échoue. Les scripts Python de `perf/` forcent eux-mêmes le format `/c/...`.
- **Bibliothèque** : lancer `make KEEP_LIB=1 clean` avant `make release` dans `pvsneslib/`. Sinon, les restes d'un build précédent passent dans la première variante (LoROM_SlowROM), par exemple des `JSL` en banque `$80`.
- **Émulation** : `perf/run_rom.py` choisit le cœur selon l'OS. Sous Windows, il prend `perf/cores/snes9x_libretro.dll`, téléchargé depuis le buildbot libretro et non versionné. La variable `SNES9X_CORE` permet de forcer un autre cœur.
- **816-opt installé dans `devkitsnes/tools`** : la dernière version du niveau 3. Les versions précédentes sont gardées à côté : `816-opt.orig.exe`, `816-opt.niveau2.exe`, `816-opt.niveau3.exe`.

## 8. Résultats de la piste 816-tcc (niveaux 1 à 3)

### Mesures (frames, `perf/tests/bench`, LoROM SlowROM, snes9x)

| Noyau | Référence | Niveau 1 | Niveau 2 | Niveau 3 | llvm-mos 16 bits (§2) |
|---|---:|---:|---:|---:|---:|
| Tri à bulles (SORT) | 101 | 100 | 92 | 65 | 24 |
| Physique (PHYSICS) | 56 | 51 | 45 | 31 | 16 |
| Tilemap (MAP) | 59 | 59 | 55 | 35 | 9 |
| Récursion (FIB18) | 26 | 26 | 23 | 21 | 17 |
| Copie (COPY) | 115 | 115 | 102 | 44 | 37 |
| Division/modulo (MATH) | 60 | 21 | 20 | 19 | 16 |
| **Total** | **417** | **372** | **337** | **215** | **119** |

En FastROM, le total du niveau 3 est de 185 frames. Détail étape par étape dans `perf/baseline_tcc.txt`. Le banc est déterministe : deux exécutions donnent une WRAM identique.

### Niveau 1 : runtime (`pvsneslib/source/libtcc.asm`, `crt0_snes.asm`)

- `tcc__mul` (16×16→16 bits) utilise le multiplicateur matériel 8×8 : un produit si les deux opérandes sont < 256, deux si l'un d'eux l'est, trois sinon. `tcc__mull` (16×16→32 bits, utilisé pour les `u32`) fait 4 produits.
- `tcc__udiv`, et donc `tcc__div`, utilise le diviseur matériel 16/8 quand le diviseur est < 256. La division par zéro donne le même résultat que la routine logicielle : quotient `$FFFF`, reste égal au dividende.
- **Verrou `tcc__hwlock`**, en RAM basse et lu en adressage long, car la NMI utilise une autre page directe. Tant qu'un calcul matériel est en cours, le code C appelé depuis une interruption passe par les routines logicielles (`*_sw`). Le verrou est mis à zéro dans `crt0`, faute de quoi la RAM (0x55 dans snes9x) bloque tout sur la voie logicielle.
- Délais d'attente comptés à la main, avec de la marge : au moins 8 cycles d'instructions entre le lancement et la lecture pour une multiplication, 16 pour une division.

### Niveau 2 : 816-opt (`tools/816-opt/src/optimizer.c`)

- **Fusion comparaison + branchement** : le booléen fabriqué dans X par chaque comparaison est remplacé par un branchement sur les drapeaux (`cmp`, ou `sec`/`sbc` pour les comparaisons signées, qui ont besoin de V). Les 10 opérateurs sont couverts, avec ou sans étiquette entre le test et le `brl`. Les comparaisons 32 bits ne sont pas touchées : un `tya` qui suit signale qu'elles ont encore besoin de Y.
- **Branchement par-dessus un `brl`** : `bXX + / brl L / + / bra M` devient `bXX M / brl L` quand M est à 28 lignes au plus, sans directive entre les deux. La transformation n'a lieu que si aucun autre branchement ne vise ce `+`.
- **Suivi des chaînes de sauts** vers la cible finale, sous forme de `jmp.w` (aussi rapide qu'un `bra` pris). Les cycles de sauts sont détectés.

### Niveau 3 : analyse de flot (`tools/816-opt/src/flow.c`)

Une passe exécutée entre deux passes du peephole. Pour chaque fonction (une section `SUPERFREE`), elle construit le graphe des sauts, étiquettes anonymes `+`/`-` comprises, et suit le mode 8 ou 16 bits de A. Elle calcule la durée de vie de A, X, Y, des drapeaux C/Z/N/V et des pseudo-registres `tcc__r0` à `tcc__f3h`. Transformations :

- **Code mort** : suppression des instructions sans effet de bord dont tous les résultats sont morts.
- **Tableaux globaux** : `lda.w #:SYM / … / lda.w #SYM + 0 / clc / adc idx / sta rK` suivi de `[rK]` devient `lda.l SYM,x` / `sta.l SYM,x`. Trois modes :
  - direct : l'indice reste dans X jusqu'au dernier accès ;
  - indice dans rK : `ldx rK` avant chaque accès, quand X n'est pas libre sur toute la plage ;
  - réutilisation : X contient déjà l'indice, par exemple dans `a[i] = b[i]`.
- **Champs de structures** : `clc / lda rK / adc #C / sta rK` suivi de `[rK]` devient `ldy #C / … [rK],y`.
- **Décalages par une constante (5 à 15 bits)** : la boucle `ldy #N / asl a / dey / bne` est déroulée, avec `xba` + `and` pour 8 bits d'un coup.
- **Valeurs disponibles** : des classes d'égalité entre A, X, Y, les pseudo-registres, les cases de pile `n,s` et les constantes. Cette passe supprime les rechargements et les stockages redondants, et remplace un `lda` par `txa` ou `tya` quand X ou Y contient déjà la valeur.
- **Registres de retour des fonctions `static`** (préfixe `tccs_`, dont l'adresse n'est jamais prise) : ils sont déduits de la durée de vie après chacun de leurs appels. Pour une fonction `void`, les registres de retour ne sont plus gardés vivants jusqu'au `rtl`.

**Modèle d'appel utilisé** (vérifié dans `816-gen.c` et dans l'asm de la bibliothèque) :

- **Arguments** : ils passent tous par la pile.
- **Valeur de retour** : dans r0 (et r0h pour la banque d'un pointeur), r1/r1h pour 32 bits, f0/f0h pour un flottant.
- **Registres lus** : un appel C ne lit aucun registre. Chaque helper `tcc__*` a ses entrées propres (`tcc__mul` : r9, r10 ; `tcc__udiv` : A, X…). Un helper inconnu est supposé tout lire.
- **Sortie inconnue** (saut indirect, `jml`) : tout est vivant.

**Prudence de l'analyse** :

- **Mode 8 bits** : une écriture 8 bits n'écrase pas le registre.
- **Blocs `.if`** : leurs instructions ne sont ni supprimées ni vues comme écrasant un registre.
- **X/Y en 8 bits** : une fonction qui passe X ou Y en 8 bits n'est pas optimisée.
- **Cases de pile**, dans la passe des valeurs :
  - une écriture invalide les cases qui la chevauchent ;
  - ce qui déplace S (`pha`, `pei`, appels, prologue) invalide toute la pile ;
  - si la fonction prend l'adresse d'une locale (`tsa`/`tsx` hors prologue), toute écriture indirecte invalide la pile.
- **Tableaux** : seul le décalage `+ 0` est réécrit. Avec `(tab + 10)[k]` et `k` négatif, l'adressage long `SYM,x` ne reboucle pas dans la banque comme le calcul 16 bits de tcc.
- **Champs** : le décalage doit être compris entre 1 et `$7FFF`, pour la même raison.

Variables d'environnement de 816-opt : `OPT816_NOFLOW=1` désactive l'analyse, `OPT816_DEBUG=1` affiche ses décisions sur stderr, `OPT816_NOPEEP2=1` montre sa sortie avant le second peephole.

### Validation

- **`perf/runall.py`** reconstruit et lance tous les tests, avec en option `--opt`, `--cc` et `--make "HIROM=1 FASTROM=1"`. Ne jamais passer `HIROM=0` : snes_rules le comprend mal.
  - Tests historiques : bench, torture, divtest, nmitest.
  - Tests ajoutés : multest (multiplication, division, 32 bits, collisions avec la NMI), cmptest (27 905 comparaisons sous 3 formes), arrtest (tableaux ; valeurs attendues calculées par `expected.py`), shifttest (675 décalages), localtest (locales et pointeurs).
  - Tous passent dans les 4 modes (LoROM/HiROM × SlowROM/FastROM).
- **`perf/abtest.py`** compile chaque exemple de `snes-examples` suivi par git avec deux chaînes, et compare les écrans à 300 frames. Pour B, les graphismes du build A sont réutilisés, car gfx4snes n'est pas déterministe. Un écart peut n'être qu'un décalage dans le temps, dû à un code plus rapide : c'était le cas pour les logos Capcom et Konami, identiques à partir de 600 frames.
- **`perf/asmtest.py`** vérifie des fragments assembleur écrits à la main (`perf/asmtests/`), pour les cas que le code C des tests ne produit pas : alias par pointeur d'une locale, chevauchement de cases de pile, `pha`. Une version volontairement privée de protection passe localtest mais échoue ici.
- **Sensibilité des tests** : chaque protection délicate a été désactivée exprès pour vérifier qu'un test la détecte (verrou matériel : 39 erreurs dans multest ; alias de pile : asmtest).
- **Jeux réels (29 septembre 2026, chaîne complète niveaux 1 à 3)** :
  - **multest dans MesenCE** (émulation au cycle près du multiplicateur et du diviseur) : `OK`, toutes les voies de `tcc__mul`, `tcc__mull` et `tcc__udiv`/`tcc__div`, avec 2 379 NMI qui calculent aussi. Les délais d'attente du niveau 1 sont donc validés. Il reste à tester sur une vraie console.
  - **geeklife** : testé par l'auteur dans **MesenCE**, fonctionne parfaitement (effets mode 7, HDMA, moteur de sprites). Mesen émule au cycle près le multiplicateur et le diviseur ; comme les scores utilisent `% 10`, `/ 10` (`tcc__udiv`, voie matérielle) et `* 10` (`tcc__mul`), les délais d'attente sont validés pour ces voies.
  - **rick1** : un sprite qui montait pendant le défilement venait d'un bug du jeu (`bclmz += 32` oublié dans `scroll_up`), et non de la chaîne. L'état de Rick suit la même séquence qu'avec la chaîne d'origine, avec une frame d'avance.
  - **rick1, chaîne complète du 30 septembre** (tcc avec division signée en ligne, 816-opt avec `byteOps` et `[rK],y`, bibliothèque niveau 1) : fonctionne parfaitement dans MesenCE, après correction de deux bugs du jeu. Le premier est `scroll_up` ; le second, un `strcpy` de `game_loadhighscores` qui débordait sur `world_start_crds` avec une SRAM vierge et faisait sauter le niveau 1 (corrigé par `memcpy(…, 10)`).
  - **Bibliothèque** : `libc_c.c` est du C compilé par tcc et 816-opt. Il faut donc reconstruire la bibliothèque (`make KEEP_LIB=1 clean`, puis `make MAKE=make release` depuis PowerShell) à chaque nouvelle version de l'un des deux.
  - **rick1 et geeklife, chaîne du 1er octobre au soir** (lectures d'octets en 16 bits, forme `volatile` à part) : fonctionnent tous les deux, y compris le zoom et la rotation en mode 7 de geeklife.
  - **rick1 et geeklife, chaîne du 1er octobre au matin** (`u8` variadiques, opérandes lus directement, calculs sur A) : fonctionnent tous les deux.
  - **rick1 et geeklife, chaîne du 1er octobre** (coût des appels : sauvegardes sur 2 octets, `return` entier sans mot de banque, plus petit point fixe des registres de retour) : fonctionnent tous les deux.
  - **rick1 et geeklife, chaîne du 30 septembre au soir** (tcc avec multiplication par une constante en ligne, bibliothèque reconstruite avec cette chaîne) : fonctionnent tous les deux.
- **Mesure en jeu réel (rick1, 29 septembre 2026)**, sur le même passage joué et le même code source (niveau 1 : marche, chute dans le puits, défilement, rocher) :

  | rick1 | Chaîne d'origine | Chaîne actuelle | Écart |
  |---|---:|---:|---:|
  | Charge médiane en marche (lignes par étape de jeu) | 127 | 90 | −29 % |
  | Durée du défilement vertical | 32 frames | 24 frames | −25 % |
  | Frames en retard (`lag_frame_counter`) | 38 | 25 | −34 % |

  Méthode :
  - Dans une **copie** de la bibliothèque, `WaitForVBlank` relève la ligne de balayage courante (`$2137`, puis `$213F`, puis `$213D` lu deux fois) dans une variable `perf_vline`, lue à chaque frame avec `RUN_ROM_WATCH`. La charge d'une étape de jeu = lignes écoulées entre le début du VBlank et cet appel.
  - La même instrumentation est ajoutée aux deux bibliothèques : celle de `develop` (compilée avec le 816-opt d'origine) et l'actuelle.
  - Dans snes9x, rick1 tourne en PAL avec overscan : 312 lignes, VBlank à la ligne 240. Le jeu avance à 30 Hz, soit deux `WaitForVBlank` par étape.
- **Profil de rick1 (chaîne actuelle, marche, lignes par étape)**, obtenu avec des marques `perf_mark` / `perf_begin` / `perf_end` ajoutées dans une copie du jeu : étape complète 106, dont `draw_entities` 103, dont animation de Rick (`gere_rick`) 53, dont collision avec la carte (`test_rick_col_map`) 36, soit un tiers de l'étape. Le reste de la boucle de `draw_entities` sur ses 13 entités coûte environ 28 lignes. Motifs coûteux relevés dans `test_rick_col_map` :
  - un pointeur local post-incrémenté (`*pt_map++`), rechargé depuis la pile et réécrit à chaque accès ;
  - la lecture d'un octet étendue à 16 bits (`lda #0 / sep / lda [r] / rep`) ;
  - un pointeur global recopié puis additionné à l'indice, au lieu de `[r],y` ;
  - une variable octet relue, étendue puis réécrite sur la pile ;
  - `x/8` signé, qui appelait `tcc__div`.
- **Suite ciblée sur rick1 (30 septembre 2026)**, en profilant `test_rick_col_map` à chaque étape (lignes par étape de marche) :

  | Version | Étape complète | `test_rick_col_map` |
  |---|---:|---:|
  | Niveau 3 installé | 106 | 36 |
  | + division signée par 2^k en ligne (tcc) | 104 | 33 |
  | + pointeur + indice octet → `[rK],y` (816-opt) | 102 | 30 |
  | + opérations sur octets en 8 bits, `inc a` pour un pointeur (816-opt) | 98 | 25 |
  | + multiplication par une constante à 2 ou 3 bits en ligne (tcc) | 90 | 27 |

  Ajouts dans `flow.c` :
  - **indice prouvé petit** : `fieldOffsets` accepte un indice dans un pseudo-registre s'il est prouvé compris entre 0 et `$7FFF`, c'est-à-dire un octet étendu à 16 bits ou un `and #N` avec N < `$8000`. Un seul `ldy` à la place de l'addition quand Y est libre jusqu'au dernier accès, sinon un `ldy` avant chaque accès ;
  - **`byteOps`** : `u8 v op= expr` (`ora`, `and`, `eor`) en 8 bits, en 5 instructions au lieu de 13 ;
  - **incrément dans A** : dans la passe des valeurs, `inc.b rX / lda.b rX` devient `inc a / sta.b rX` quand A vaut déjà rX.

  Tests ajoutés : `sdivtest`, `ptrtest` (dont les indices négatifs, qui ne doivent pas être transformés) et `bytetest`. abtest contre la chaîne installée : 16 ROMs identiques, 54 écrans identiques, aucune différence.
- **Bugs attrapés par les tests pendant le développement** : réécritures de tableaux qui se chevauchaient sur X (tri faux), indicateur « X vivant » écrasé (cmptest faux), variables globales partagées entre la NMI et le programme dans un test.

### Pièges et bugs rencontrés (existaient avant ce travail)

- **gfx4snes n'est pas déterministe** : la map en mode 5 (`-M 5`) change d'un build à l'autre, et la conversion de `sprite16.bmp` plante par intermittence. `thirdparty/maps/DynamicMap` ne se construit jamais.
- **Variables non mises à zéro** : les statiques non initialisées ne le sont pas en HiROM, et certaines globales ne le sont pas non plus (WRAM à 0x55 dans snes9x). Les tests initialisent donc explicitement leurs variables.
- **Pile de tcc limitée** : tcc ne gère pas plus de 255 octets de variables locales par fonction, l'adressage `d,s` étant sur 8 bits. L'assembleur échoue alors avec « Out of 8-bit range ».
- **`make release`** ne fait pas de `clean` au préalable (voir §7).
- **Callbacks VBlank (`nmiSet`)** : appeler `consoleVblank()` en premier. Une écriture en VRAM faite après la fin du VBlank est ignorée par la console et par Mesen, alors que snes9x l'accepte : la première version de multest s'affichait dans snes9x mais pas dans Mesen.
- **`showFPScounter`** (videos.asm) utilisait le diviseur matériel sans verrou. **Corrigé** : il prend `tcc__hwlock`, et divise par 10 en logiciel si le diviseur est occupé. Les deux voies affichent le même résultat au pixel près.

### Limites connues

- Restent logiciels : les divisions par un diviseur ≥ 256, les divisions 32 bits (`tcc__divl`/`tcc__udivl`) et les multiplications appelées depuis une interruption pendant un calcul matériel.
- L'affinage des registres de retour ne concerne que les fonctions `static` ; pour les autres, main compris, il reste prudent.
- Le bug de tcc sur les `u8` variadiques (§3) est corrigé depuis le 1er octobre (`vatest`) : COPY affiche 17.

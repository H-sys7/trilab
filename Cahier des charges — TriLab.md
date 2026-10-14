# Cahier des charges — TriLab

Apprendre tous les algorithmes de tri en C, par le code.

Oct 13, 2026 · @Durant

## 1. Contexte et objectif

TriLab est un projet d'apprentissage : l'apprenant code lui-même 16 algorithmes de tri en C, du tri à bulles au Timsort, sans jamais recevoir la solution.

La règle de base est simple : on comprend un algorithme en l'écrivant, pas en lisant un cours. Le dépôt fournit des squelettes de code, des tests et des outils de diagnostic. L'apprenant écrit le corps des fonctions.

**Objectifs**

- Comprendre chaque algorithme en profondeur, en le construisant étape par étape.
- Apprendre le C au moment où un algorithme en a besoin : pointeurs, `malloc`, `struct`, `void *`, pointeurs de fonction.
- Comprendre la complexité en la mesurant soi-même, puis en la nommant.
- Finir avec une bibliothèque de tri réutilisable (`libtri`) et un comparatif chiffré de ses propres implémentations.

**Deux lecteurs pour ce document**

- L'apprenant : la vision et les règles (sections 1 à 3, 10, 12 et annexe A).
- Claude Code : la spécification à générer (sections 4 à 9, 11, 13, 14 et annexes B et C).

## 2. Public visé et prérequis

L'apprenant connaît les bases du C (variables, boucles, fonctions, tableaux) mais les pointeurs sont encore flous. Il code déjà dans d'autres langages (TypeScript, React).

Conséquences sur la progression :

- Un exercice 00 d'échauffement sur les pointeurs passe avant le premier tri.
- `malloc` n'arrive qu'à l'exercice 08 (tri fusion), quand l'algorithme en a vraiment besoin.
- Le C générique (`void *`, comparateurs) n'arrive qu'à l'exercice 16, une fois les algorithmes maîtrisés sur des `int`.

Chaque exercice est indépendant. L'apprenant avance à son rythme, sans date imposée.

Poste de travail : Windows avec WSL2 et Ubuntu (détails en section 5).

## 3. Principes pédagogiques (non négociables)

Sept règles guident toute la conception. Si une décision technique entre en conflit avec l'une d'elles, la règle gagne.

1. **Zéro solution dans le dépôt.** Ni dans les fichiers, ni dans les tests, ni dans l'historique git.
2. **Le code avant la théorie.** Pas de cours. Le texte se limite à des commentaires courts dans les fichiers. Seule exception : la fiche complexité, à lire après l'exercice 07 (section 10).
3. **Une notion C arrive quand un algorithme en a besoin.** Jamais avant.
4. **Étape par étape.** Chaque algorithme est découpé en petites fonctions imposées, testées séparément et dans l'ordre.
5. **Un retour qui pointe la cause.** Un message d'échec dit ce qui est observé et pose une question. Il ne dit jamais comment corriger.
6. **La solution seulement sur demande explicite**, et uniquement auprès du relecteur IA.
7. **Bibliothèque standard C11 uniquement** dans le code C. Les outils autour (bash, make, valgrind, gdb) sont autorisés.

## 4. Progression des exercices

Le parcours compte 20 exercices (00 à 19) répartis en 10 phases. Il va des tris simples sur des `int` jusqu'à une bibliothèque générique complète.

| # | Phase | Algorithme | Notion C introduite | Étapes imposées (fonctions) |
| --- | --- | --- | --- | --- |
| 00 | Démarrage | Échauffement (pas un tri) | Adresse `&`, déréférencement `*`, passage par adresse | `swap`, `min_index`, `reverse`, `is_sorted` |
| 01 | Tris simples | Tri à bulles | Tableau en paramètre (`int *a, size_t n`), `size_t` | `bubble_pass` (une passe, renvoie le nombre d'échanges), `bubble_sort` (arrêt anticipé) |
| 02 | Tris simples | Tri par sélection | Invariant de boucle, indices | `min_index_from`, `selection_sort` |
| 03 | Tris simples | Tri par insertion | Boucle à double condition, piège du `size_t` qui ne descend pas sous 0 | `insert_into_sorted` (insère `a[len]` dans `a[0..len)` déjà trié), `insertion_sort` |
| 04 | Améliorer les simples | Tri cocktail | Bornes qui se resserrent | `cocktail_forward`, `cocktail_backward`, `cocktail_sort` |
| 05 | Améliorer les simples | Tri à peigne | Calcul flottant puis conversion en entier | `comb_next_gap`, `comb_pass`, `comb_sort` |
| 06 | Améliorer les simples | Tri de Shell | Tableau `static const` (suite d'écarts fournie) | `gapped_insertion`, `shell_sort` |
| 07 | Mesurer | Mesure et complexité | Pointeur de fonction, première `struct`, `clock()`, `FILE *` | `fill_random`, `fill_sorted`, `fill_reversed`, `fill_nearly_sorted`, `time_sort`, `insertion_sort_counted`, `bench_write_csv` |
| 08 | Diviser pour régner | Tri fusion | `malloc` / `free`, récursion, tableau auxiliaire | `merge`, `merge_sort_rec`, `merge_sort` (une seule allocation) |
| 09 | Diviser pour régner | Tri rapide | Récursion, partition, choix du pivot | `partition_last`, `quick_sort_naive`, `median_of_three`, `quick_sort` |
| 10 | Arbres | Tri par arbre binaire de recherche | `struct` récursive, pointeur vers pointeur (`struct node **`), libération récursive | `bst_insert`, `bst_to_array` (parcours infixe), `bst_free`, `tree_sort` |
| 11 | Arbres | Tri par arbre AVL | Hauteur, rotations, fonction qui renvoie la nouvelle racine | `avl_height`, `avl_rotate_left`, `avl_rotate_right`, `avl_insert`, `avl_sort` |
| 12 | Arbres | Tri par tas | Arbre rangé dans un tableau (indices parent / enfants) | `sift_down`, `build_max_heap`, `heap_sort` |
| 13 | Sans comparaison | Tri par comptage | `calloc`, erreur signalée par code retour | `find_min_max`, `count_occurrences`, `prefix_sums`, `counting_sort` (renvoie `0` ou `-1`) |
| 14 | Sans comparaison | Tri par base (radix LSD) | Opérations bit à bit, `unsigned`, entiers négatifs | `radix_byte`, `radix_pass`, `radix_sort` |
| 15 | Sans comparaison | Tri par seaux | `double`, tableau dynamique avec `realloc` | `bucket_index`, `bucket_push`, `bucket_sort` (valeurs dans \[0, 1)) |
| 16 | Générique | Tri générique façon `qsort` | `void *`, octets, `memcpy`, comparateur `int (*)(const void *, const void *)` | `generic_swap`, `cmp_int`, `cmp_double`, `cmp_student`, `insertion_sort_generic`, `merge_sort_generic` |
| 17 | Hybrides | Introsort | Assembler des fonctions existantes, limite de profondeur | `depth_limit`, `introsort_loop`, `introsort` |
| 18 | Hybrides | Timsort simplifié | Pile de structures, stabilité | `count_run`, `compute_minrun`, `merge_runs`, `timsort` |
| 19 | Projet final | `libtri` et ses outils | Compilation séparée, bibliothèque statique, `argc` / `argv`, lecture de fichier | Voir section 11 |

**Règles communes**

- Les exercices 01 à 14 travaillent sur des `int`, le 15 sur des `double`. À partir du 16, les tris par comparaison deviennent génériques.
- Un exercice qui échange des éléments réécrit son propre `swap` en `static` dans son `.c`. La répétition est voulue (3 lignes).
- Chaque exercice peut avoir des bonus optionnels, testés avec `make test BONUS=1`. Exemples : position du dernier échange (01), fusion itérative (08), partition à trois voies et partition de Hoare (09), insertion itérative dans l'ABR (10).

## 5. Environnement technique (Windows + WSL2)

Tout tourne dans Ubuntu sous WSL2, où gcc, les sanitizers et Valgrind fonctionnent comme sur un Linux classique.

| Élément | Choix | Pourquoi |
| --- | --- | --- |
| Système | Windows 10 ou 11, WSL **2**, Ubuntu 24.04 LTS | ASan et Valgrind ne marchent pas correctement sous WSL1 |
| Compilateur | `gcc`, norme `-std=c11` | Inclut AddressSanitizer et UndefinedBehaviorSanitizer |
| Outils | `make`, `gdb`, `valgrind`, `timeout` (coreutils) | Build, débogage, mémoire, détection de boucle infinie |
| Emplacement du projet | `~/trilab`, dans le système de fichiers Linux | `/mnt/c/...` est lent et pose des problèmes de droits |
| Éditeur | VS Code ou Cursor, connecté à WSL | Terminal et éditeur voient les mêmes fichiers |

Installation, à faire une fois :

```bash
wsl -l -v                      # dans PowerShell : la colonne VERSION doit afficher 2
sudo apt update
sudo apt install build-essential gdb valgrind
```

La cible `make doctor` vérifie tout cela automatiquement : version de WSL, présence de chaque outil, et un mini-programme compilé avec ASan pour prouver que les sanitizers marchent.

## 6. Architecture du projet

Un dossier par exercice, des règles de build partagées et un mini-framework de test maison. L'apprenant ne modifie qu'un seul fichier par exercice.

```text
trilab/
├── Makefile                    # racine : délègue aux exercices (EX=01), status, doctor, bench
├── README.md                   # 1 page : installer, commandes make, règles du jeu
├── .gitignore                  # build/, *.o, paquets de relecture
├── .checksums                  # empreintes sha256 des fichiers « ne pas modifier »
├── mk/
│   ├── config.mk               # compilateur et drapeaux par mode (base, check, memcheck, bench)
│   └── exercise.mk             # règles communes incluses par chaque exercice
├── tools/
│   ├── doctor.sh               # vérifie WSL2, gcc, gdb, valgrind, ASan
│   ├── run_tests.sh            # 1 test = 1 processus, timeout, détection de crash
│   ├── review.sh               # assemble le paquet pour le relecteur IA
│   ├── bench.sh                # 1 mesure = 1 processus (survit à un débordement de pile)
│   ├── status.sh               # tableau de progression des 20 exercices
│   └── verify_integrity.sh     # compare les fichiers protégés à .checksums
├── tests/framework/
│   ├── tl_test.h               # mini-framework : déclaration des tests, assertions, sortie
│   ├── tl_check.h              # oracles : trié, permutation, stable, tas valide, ABR valide, AVL équilibré
│   └── tl_rand.h               # générateur pseudo-aléatoire maison (xorshift), graine fixe
├── docs/
│   ├── RELECTEUR_IA.md         # instructions du relecteur IA (annexe B)
│   └── complexite.md           # fiche complexité, à lire après l'exercice 07 (annexe A)
├── exercises/
│   ├── 00_echauffement/
│   │   ├── Makefile            # 3 lignes : nom, sources, include ../../mk/exercise.mk
│   │   ├── echauffement.h      # signatures imposées et contrats (ne pas modifier)
│   │   ├── echauffement.c      # TODO à compléter : seul fichier modifié par l'apprenant
│   │   └── test_echauffement.c # tests de l'exercice (ne pas modifier)
│   ├── 01_bulle/               # bubble.h, bubble.c, test_bubble.c, Makefile
│   ├── 02_selection/
│   ├── 03_insertion/
│   ├── 04_cocktail/
│   ├── 05_peigne/
│   ├── 06_shell/
│   ├── 07_mesure/              # + bench_main.c (fourni) et RESULTATS.md (rempli par l'apprenant)
│   ├── 08_fusion/
│   ├── 09_rapide/
│   ├── 10_arbre_abr/
│   ├── 11_arbre_avl/
│   ├── 12_tas/
│   ├── 13_comptage/
│   ├── 14_radix/
│   ├── 15_seaux/
│   ├── 16_generique/
│   ├── 17_introsort/
│   └── 18_timsort/
└── final/
    ├── Makefile
    ├── include/libtri.h        # API publique unique
    ├── src/                    # un .c par algorithme, porté depuis les exercices
    ├── cli/tri.c               # outil en ligne de commande
    ├── cli/tri_bench.c         # comparatif de tous les algorithmes et de qsort
    ├── tests/                  # tests de la bibliothèque
    └── RAPPORT.md              # rapport final rédigé par l'apprenant
```

Chaque dossier d'exercice contient aussi un `build/` créé à la compilation et ignoré par git.

## 7. Anatomie d'un exercice

Un exercice = 4 fichiers. Seul le `.c` de l'algorithme est modifié par l'apprenant ; les 3 autres sont protégés par `.checksums`.

| Fichier | Contenu | Modifiable |
| --- | --- | --- |
| `<algo>.h` | Signatures imposées + contrat de chaque fonction (1 à 3 lignes) | Non |
| `<algo>.c` | En-tête court, fonctions vides qui compilent, marqueurs `TODO(étape N)` | Oui |
| `test_<algo>.c` | Tests groupés par étape, du cas le plus simple au plus dur | Non |
| `Makefile` | 3 lignes : nom de l'exercice, sources, `include ../../mk/exercise.mk` | Non |

**Règles d'écriture des commentaires**

- En-tête du `.c` : 6 lignes maximum. Nom de l'exercice, notion C nouvelle (1 ligne), liste des étapes, commandes utiles.
- Contrat dans le `.h` : il décrit le résultat attendu et ses propriétés. Il ne décrit jamais les boucles, les indices ni l'ordre des opérations.
- Une contrainte imposée s'écrit comme une contrainte : « une seule allocation », « sans tableau auxiliaire ».

**Règles des fonctions vides (stubs)**

- Chaque stub compile sans aucun avertissement, même en mode `check` (`-Werror`).
- Les paramètres sont neutralisés par `(void)x;` et la valeur de retour est neutre (`0`, `NULL`).
- Un helper `static` pas encore utilisé est référencé par `(void)nom;` dans un stub, sinon `-Wunused-function` bloque la compilation.
- Sur un stub intact, `make test` échoue proprement : aucun crash, aucun timeout.

**Exemple : le contrat de l'exercice 01**

```c
/* Exercice 01 — Tri à bulles. NE PAS MODIFIER CE FICHIER. */
#ifndef BUBBLE_H
#define BUBBLE_H

#include <stddef.h>

/* Étape 1. Une passe sur a[0..n) : chaque paire de voisins mal ordonnée,
 * rencontrée de gauche à droite, est échangée.
 * Après l'appel, le plus grand élément est en a[n - 1].
 * Renvoie le nombre d'échanges. a peut valoir NULL si n == 0. */
size_t bubble_pass(int *a, size_t n);

/* Étape 2. Trie a[0..n) par ordre croissant en répétant des passes.
 * S'arrête dès qu'une passe ne fait aucun échange. */
void bubble_sort(int *a, size_t n);

#endif
```

**Exemple : le squelette à compléter**

```c
/* Exercice 01 — Tri à bulles
 * Notion C : un tableau passé à une fonction devient un pointeur vers son 1er élément.
 * Étapes : 1) bubble_pass   2) bubble_sort
 * Commandes : make test · make check · make review
 */
#include "bubble.h"

/* TODO(étape 1) : échange les valeurs pointées par x et y. */
static void swap(int *x, int *y)
{
    (void)x;
    (void)y;
}

size_t bubble_pass(int *a, size_t n)
{
    /* TODO(étape 1) */
    (void)swap;
    (void)a;
    (void)n;
    return 0;
}

void bubble_sort(int *a, size_t n)
{
    /* TODO(étape 2) */
    (void)a;
    (void)n;
}
```

## 8. Système de correction

Décision : on combine les 4 options proposées, en 5 couches, car chacune attrape une famille d'erreurs que les autres ratent. Un exercice est **validé** quand `make check` passe, plus `make memcheck` à partir de l'exercice 08.

&#91;embedded content: boucle de correction · 3 vérifications, 2 retours au code\]

Pour les exercices 00 à 07, l'exercice est validé dès que `make check` passe ; `make memcheck` s'ajoute à partir de l'exercice 08.

| Couche | Ce qu'elle attrape | Quand | Cible |
| --- | --- | --- | --- |
| 1. Compilation stricte | Types, comparaisons signé / non signé, variables inutilisées, `return` oublié | À chaque build | `make` (avertissements), `make check` (erreurs) |
| 2. Tests par étapes | Résultat faux, valeurs perdues ou dupliquées, cas limites, boucle infinie, crash | À chaque essai | `make test` |
| 3. ASan + UBSan | Accès hors du tableau, use-after-free, double `free`, fuites, débordement d'entier signé | Avant de valider | `make check` |
| 4. Valgrind | Lecture de mémoire non initialisée, fuites (second avis) | Dès l'exercice 08 | `make memcheck` |
| 5. Relecteur IA | Logique, compréhension, style | Quand on bloque après les couches 1 à 4 | `make review` |

**Pourquoi cette combinaison**

- Les avertissements coûtent zéro : ils signalent des erreurs avant même l'exécution.
- Les tests découpés par étape vérifient que c'est le bon algorithme, pas seulement que le résultat est trié.
- Un débordement d'un case (off-by-one) peut passer les tests en silence. ASan le rend visible, avec la ligne exacte.
- Valgrind n'est pas un doublon : avec gcc, ASan ne voit pas les lectures de mémoire non initialisée. Les deux ne s'exécutent pas sur le même binaire, d'où deux cibles.
- L'IA vient en dernier : elle explique le « pourquoi » quand les outils ont donné le « où ».

**Drapeaux de compilation (`mk/config.mk`)**

| Mode | Drapeaux |
| --- | --- |
| base | `-std=c11 -Wall -Wextra -pedantic -Wshadow -Wvla -Wstrict-prototypes -Wmissing-prototypes -g -O0` |
| check | base + `-Werror -Wconversion -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer` |
| memcheck | base, exécuté avec `valgrind --leak-check=full --track-origins=yes --error-exitcode=42` |
| bench | `-std=c11 -O2 -DNDEBUG` |

Le mode base reste tolérant pour ne pas décourager au début. Le mode check est exigeant car c'est lui qui valide.

**Lanceur de tests (`tools/run_tests.sh`)**

- Chaque binaire de test accepte `--list` (liste les tests) et `<nom>` (lance un seul test).
- Chaque test tourne dans son propre processus, sous `timeout 5s` (durée réglable par test). Un crash n'efface donc pas les autres résultats.
- Codes de sortie interprétés : `0` réussi, `1` échec, `124` temps dépassé, `139` plantage mémoire, `23` erreur ASan, `24` erreur UBSan (via `ASAN_OPTIONS` et `UBSAN_OPTIONS`).
- Pour une erreur ASan ou UBSan, le lanceur n'affiche que les lignes du rapport qui pointent vers le fichier de l'apprenant.
- Seule la première étape en échec est détaillée. Les étapes suivantes affichent leur score et « en attente ».
- Dernière ligne : score global et prochaine étape à travailler. Code de sortie non nul si un test échoue.

**Oracles (`tests/framework/tl_check.h`)**

Ils transforment un résultat faux en symptôme précis, sans rien révéler de l'implémentation : tableau trié, permutation de l'entrée (valeur perdue ou dupliquée), tableau inchangé, premier indice différent, stabilité (clé + indice d'origine), propriété de tas, propriété d'ABR, équilibre AVL.

**Cas testés pour chaque tri**

1. `n = 0` avec `a = NULL`
2. 1 élément, puis 2 éléments dans l'ordre et inversés
3. Déjà trié, puis trié à l'envers
4. Doublons, puis tous égaux
5. Négatifs, `INT_MIN` et `INT_MAX`
6. Aléatoire, `n = 1000`, graine fixe, comparé à `qsort` de la libc
7. Grand `n` adapté à l'algorithme, sous le timeout

S'y ajoutent les tests d'étape : un état attendu écrit en dur et vérifié à la main (exemple : le tableau après une seule passe).

**Règles des messages d'échec** (exemples en annexe C)

1. Nommer l'étape, la fonction et le cas.
2. Montrer l'entrée, l'attendu et l'obtenu (tableaux de 20 éléments maximum, sinon une fenêtre autour du premier écart).
3. Donner le symptôme trouvé par l'oracle.
4. Finir par une question qui pointe vers la cause.
5. Jamais de code corrigé, de borne exacte ni d'opérateur à remplacer.

**Cibles make**

| Cible | Effet |
| --- | --- |
| `make` | Compile en mode base |
| `make test` | Lance les tests (`BONUS=1` ajoute les bonus) |
| `make check` | Intégrité + mode check + tous les tests : critère de validation |
| `make memcheck` | Tous les tests sous Valgrind |
| `make debug T=<test>` | Ouvre gdb sur un test précis |
| `make review` | Prépare le paquet pour le relecteur IA |
| `make clean` | Supprime `build/` |

Depuis la racine : les mêmes cibles avec `EX=01`, plus `make doctor`, `make status` (progression des 20 exercices) et `make bench` (section 10).

## 9. Relecteur IA

Le relecteur est Claude, dans un Projet Claude dédié « TriLab — Relecteur » dont les instructions sont le contenu de `docs/RELECTEUR_IA.md` (annexe B). Le projet C n'appelle aucune API : il reste sans dépendance.

**Préparer une relecture : `make review`**

La cible crée `build/review_<NN>.md`, qui regroupe tout ce dont l'IA a besoin :

- le contrat de l'exercice (`.h`) et le code de l'apprenant (`.c`) ;
- la sortie de compilation en mode check ;
- la sortie de `make test`, et celle des sanitizers en cas d'erreur mémoire ;
- deux zones à remplir : « Ma question » et « Ce que j'ai déjà essayé ».

Sous WSL, le script copie aussi le paquet dans le presse-papiers Windows avec `clip.exe`, après conversion en UTF-16 pour garder les accents.

**Les 4 niveaux d'indice**

| Niveau | Ce que l'IA donne | Exemple |
| --- | --- | --- |
| N1 — Où regarder | Une question qui oriente vers la bonne zone | « Ton problème vient de l'étape 1. Que fait ta boucle sur le dernier tour ? » |
| N2 — La notion | Le nom du concept en jeu + une question | « C'est une question de borne de boucle. Combien de paires de voisins existe-t-il dans 5 éléments ? » |
| N3 — Le cas concret | Un mini-exemple à dérouler à la main | « Prends {3, 1, 2}. Écris la valeur de i et du tableau à chaque tour. » |
| N4 — En mots | Ce que doit faire la partie fautive, en français, sans code ni opérateur | « La boucle doit examiner chaque paire de voisins, y compris la toute dernière. » |

**Règles du relecteur**

- Il commence toujours au niveau N1 et donne un seul niveau par réponse.
- Il monte d'un niveau sur demande (« indice suivant ») ou si une nouvelle tentative échoue au même endroit.
- S'il y a plusieurs bugs, il ne traite que le premier, dans l'ordre des étapes.
- Il peut citer une seule ligne du code de l'apprenant pour pointer un endroit. Il ne la réécrit jamais.
- Il termine par une question qui vérifie la compréhension.
- Il donne la solution uniquement si l'apprenant écrit explicitement « SOLUTION ». Il fournit alors la fonction corrigée et explique chaque choix, puis conseille de la recoder de mémoire plus tard.
- Sur un exercice validé, il propose une question de compréhension (exemple : « pourquoi ton tri est-il stable ? »).

Ce protocole s'applique aussi quand l'apprenant colle son code et ses erreurs directement dans une conversation, sans passer par `make review`.

## 10. Module complexité

La complexité s'apprend en 3 temps : on la **mesure** (exercice 07), on la **nomme** (fiche lue juste après), puis on la **vérifie** à chaque nouvelle phase.

**Mesurer — exercice 07**

Placé entre les tris simples et les tris rapides : l'apprenant voit le mur du « lent » avant de découvrir comment le franchir.

- Il écrit 4 générateurs de données : aléatoire, trié, inversé, presque trié.
- Il écrit `time_sort`, qui chronomètre un tri reçu en paramètre (`typedef void (*sort_fn)(int *, size_t);`).
- Il compte les opérations d'un tri par insertion avec `struct op_stats { size_t comparisons; size_t shifts; }`. Compter enlève le bruit de la machine.
- Il écrit l'export CSV : `algo,profil,n,temps_ms,ratio`.
- Il remplit `RESULTATS.md` : ses **prédictions avant** de lancer, puis ses observations et les écarts.

**Le banc de mesure — `make bench`**

- `bench_main.c` (fourni) contient le registre des tris. Seuls les exercices validés y sont inclus, via des macros `-DWITH_<NOM>`.
- `n` part de 1 000 et double. Un algorithme arrête de grandir dès qu'une mesure dépasse 2 s.
- Chaque point est la médiane de 5 mesures.
- Le terminal affiche une colonne **ratio** = temps(2n) / temps(n). C'est l'outil central : ≈ 2 veut dire linéaire, ≈ 4 veut dire quadratique.
- `tools/bench.sh` lance chaque couple (algorithme, profil) dans son propre processus. Un débordement de pile devient un résultat lisible (« pile dépassée à n = … »), pas un plantage.
- Le tri par seaux a son propre profil : des `double` dans \[0, 1).

**Nommer — la fiche `docs/complexite.md`**

Deux pages maximum, à lire une fois l'exercice 07 validé. Son contenu est celui de l'annexe A. C'est la seule exception à la règle « pas de cours ».

**Vérifier — après chaque phase**

L'apprenant relance `make bench` et ajoute une partie à `RESULTATS.md` :

- Après 09 : à partir de quel `n` le tri fusion bat-il l'insertion ? Que fait le tri rapide naïf sur une entrée déjà triée ?
- Après 12 : l'ABR et l'AVL sur une entrée déjà triée. Pourquoi un tel écart ?
- Après 15 : le comptage et le radix sont-ils vraiment linéaires ? Que coûterait le comptage si les valeurs allaient de 0 à un milliard ?
- Après 18 : Timsort sur une entrée presque triée, comparé au tri fusion.

Le rapport final (section 11) rassemble tout.

## 11. Projet final : `libtri`

L'apprenant rassemble ses 16 algorithmes dans une bibliothèque statique à une seule API, puis écrit deux outils qui l'utilisent. Il reprend son propre code des exercices et l'adapte.

**API publique (`final/include/libtri.h`, fournie)**

```c
#include <stdbool.h>
#include <stddef.h>

typedef int (*tri_cmp)(const void *, const void *);

typedef enum {
    TRI_BUBBLE, TRI_SELECTION, TRI_INSERTION, TRI_COCKTAIL, TRI_COMB, TRI_SHELL,
    TRI_MERGE, TRI_QUICK, TRI_TREE, TRI_AVL, TRI_HEAP,
    TRI_COUNTING, TRI_RADIX, TRI_INTROSORT, TRI_TIMSORT,
    TRI_ALGO_COUNT
} tri_algo;

typedef enum { TRI_OK = 0, TRI_EINVAL, TRI_ENOMEM, TRI_EUNSUPPORTED } tri_status;

/* Tri générique façon qsort. Comptage et radix renvoient TRI_EUNSUPPORTED. */
tri_status tri_sort(void *base, size_t n, size_t size, tri_cmp cmp, tri_algo algo);

/* Tri d'entiers : accepte tous les algorithmes, y compris comptage et radix. */
tri_status tri_sort_int(int *a, size_t n, tri_algo algo);

/* Tri par seaux : valeurs dans [0, 1). */
tri_status tri_sort_unit_doubles(double *a, size_t n);

const char *tri_algo_name(tri_algo algo);
bool tri_algo_is_stable(tri_algo algo);
```

**Ce que l'apprenant écrit**

- `src/` : un `.c` par algorithme, compilé séparément, puis assemblé en `build/libtri.a` avec `ar`.
- `cli/tri.c` : `tri [-a ALGO] [-r] [-s] [-l] [-h] [FICHIER]`. Lit un entier par ligne (fichier ou entrée standard) et écrit le résultat trié. `-r` inverse l'ordre, `-s` refuse un algorithme instable, `-l` liste les algorithmes.
- L'analyse de `argv` se fait à la main : `getopt` est POSIX, pas C standard.
- Erreurs : ligne non entière → message avec le numéro de ligne et code `2` ; fichier introuvable → code `1`.
- `cli/tri_bench.c` : tous les algorithmes et `qsort` de la libc, sur 5 profils (aléatoire, trié, inversé, presque trié, beaucoup de doublons). Mêmes règles que `make bench`. Export CSV.

**Tests fournis (`final/tests/`)**

- Chaque algorithme passe par l'API avec les mêmes oracles que les exercices.
- Stabilité vérifiée pour chaque algorithme déclaré stable par `tri_algo_is_stable`.
- Codes d'erreur : `base == NULL` avec `n > 0`, `size == 0` et combinaisons non supportées.
- Tests de la ligne de commande : entrées valides, ligne invalide, fichier absent, option inconnue.

**Rapport final (`final/RAPPORT.md`)**

1. Un tableau par algorithme : complexité théorique et ratio mesuré.
2. Trois surprises rencontrées pendant le projet, et leur explication.
3. L'algorithme choisi dans 4 situations, avec la raison : données presque triées, mémoire très limitée, stabilité obligatoire, entiers dans une petite plage.

Le projet est terminé quand `make -C final check` passe, que `tri-bench` a tourné et que le rapport est rempli.

## 12. Améliorations apportées à la demande initiale

La direction reste la même : apprendre par le code, sans solution. Ces 15 ajouts rendent cette direction applicable au quotidien.

| Amélioration | Pourquoi |
| --- | --- |
| Exercice 00 d'échauffement sur les pointeurs | Les pointeurs sont encore flous : mieux vaut les isoler avant le premier tri |
| Chaque algorithme découpé en fonctions imposées, testées une par une | Rend le « étape par étape » concret et vérifie que c'est le bon algorithme, pas juste un résultat trié |
| 1 test = 1 processus, avec timeout | Un crash ou une boucle infinie n'efface plus tous les autres retours |
| Oracles de symptômes (permutation, stabilité, tas…) | Permet des messages précis sur la cause, sans montrer le code attendu |
| Deux niveaux de compilation : base et check | Tolérant pendant l'écriture, strict pour valider |
| Valgrind en complément d'ASan, dès l'exercice 08 | Il voit la mémoire non initialisée, ce que l'ASan de gcc ne voit pas |
| `make review` + 4 niveaux d'indice | L'IA reçoit tout le contexte d'un coup et ses indices restent cohérents |
| Module complexité par la mesure, placé avant les tris rapides | On sent le problème du quadratique avant de découvrir la solution |
| Arbres (ABR puis AVL) placés avant le tas | Le tas devient « un arbre rangé dans un tableau » ; l'ABR montre en vrai la dégénérescence |
| Variantes regroupées : cocktail, peigne, Shell, puis introsort et Timsort | Chaque variante améliore un algorithme déjà codé ; les hybrides réutilisent les pièces existantes |
| Code générique repoussé à l'exercice 16 | Une difficulté à la fois : d'abord l'algorithme, ensuite `void *` et les comparateurs |
| Bonus optionnels (`BONUS=1`) | Aller plus loin sans bloquer la progression |
| `make doctor` et réglages spécifiques à WSL | Évite de perdre une séance sur l'installation |
| Générateur aléatoire maison dans les tests | Résultats identiques d'une exécution à l'autre, donc échecs reproductibles |
| Protection anti-fuite de solution pendant la génération (section 14) et `.checksums` | La règle « zéro solution » est garantie dans les faits, pas seulement promise |

## 13. Livrables, lots et critères d'acceptation

Le projet est généré en 8 lots. Le lot 1 est le livrable immédiat ; chaque lot suivant attend la validation du précédent.

| Lot | Contenu |
| --- | --- |
| 1 — Livrable immédiat | Squelette complet : Makefile racine, `mk/`, `tools/`, `tests/framework/`, `docs/RELECTEUR_IA.md`, README, `.gitignore`, `.checksums`. Exercices 00 et 01 prêts à coder |
| 2 | Exercices 02 à 06 |
| 3 | Exercice 07, `bench_main.c`, `tools/bench.sh`, `docs/complexite.md` |
| 4 | Exercices 08 et 09 |
| 5 | Exercices 10 à 12 |
| 6 | Exercices 13 à 15 |
| 7 | Exercices 16 à 18 |
| 8 | `final/` complet |

**Critères d'acceptation, à vérifier pour chaque lot**

- [ ] `make doctor` passe sur un Ubuntu 24.04 neuf sous WSL2, avec seulement `build-essential`, `gdb` et `valgrind` installés.
- [ ] Chaque stub compile sans aucun avertissement, en mode base et en mode check.
- [ ] Les fichiers de test compilent eux aussi sans avertissement en mode check.
- [ ] Sur un stub intact, `make test` échoue proprement : ni crash du lanceur, ni timeout.
- [ ] Avec l'implémentation de référence privée, `make check` et `make memcheck` passent à 100 %.
- [ ] Chaque mutant privé (boucle trop courte, valeur perdue, boucle infinie, accès hors tableau, fuite) déclenche le diagnostic attendu.
- [ ] Aucune solution dans le dépôt, ni dans l'historique git.
- [ ] En-têtes de `.c` de 6 lignes maximum, aucun texte théorique.
- [ ] Tous les messages sont en français, sans code corrigé.
- [ ] `.checksums` est à jour et `tools/verify_integrity.sh` passe.

## 14. Consignes pour Claude Code

Ces règles s'ajoutent à tout le reste du document. En cas de doute, la règle « zéro solution » l'emporte.

1. Lis ce cahier en entier avant de générer. Génère un seul lot à la fois, puis attends la validation.
2. N'écris jamais le corps d'une fonction marquée `TODO`. Les stubs suivent les règles de la section 7.
3. Pour valider tes tests, écris une implémentation de référence dans `/tmp/trilab_ref/`, hors du dépôt. Ne la commite jamais et supprime-la à la fin du lot. Même règle pour les mutants.
4. Les attendus des tests d'étape sont des tableaux écrits en dur et vérifiés à la main. Les attendus des tests globaux viennent d'un oracle (`qsort` de la libc et `tl_check.h`).
5. Les messages d'échec suivent l'annexe C. Un message ne contient jamais de code C corrigé, de borne exacte ni d'opérateur à remplacer.
6. Tout fichier `.c` ou `.h` n'utilise que la bibliothèque standard C11. POSIX est autorisé uniquement dans `tools/*.sh`.
7. Identifiants en anglais, commentaires et messages en français, fichiers en UTF-8.
8. Style : 4 espaces, accolades à la ligne pour les fonctions, 100 colonnes maximum.
9. Ne modifie jamais un fichier `<algo>.c` déjà livré : il appartient à l'apprenant.
10. Mets à jour `.checksums` à chaque lot, pour les fichiers protégés uniquement.
11. À la fin d'un lot, livre : la liste des fichiers créés, la sortie de `make doctor` et celle de `make status`. Ne montre aucun extrait de l'implémentation de référence.

## 15. Hors périmètre

Ces sujets sont volontairement exclus pour garder le projet centré sur les algorithmes et le C.

- Interface graphique ou animation des tris.
- Tris parallèles (threads) et tris de réseaux (bitonique).
- Tris externes, pour des fichiers plus gros que la mémoire.
- « Galloping » complet de Timsort.
- Windows natif, MSVC et macOS.
- Appel d'une API d'IA depuis le projet.
- Preuves mathématiques formelles de complexité.

## Annexe A — La complexité, en clair

La complexité ne mesure pas des secondes : elle dit comment le travail grandit quand la quantité de données grandit.

*Conseil : lis cette annexe après l'exercice 07, pour garder la découverte par la mesure. Ce texte sert de base à `docs/complexite.md`.*

**L'idée, avec une image**

Tu dois ranger un paquet de n cartes. Méthode A : pour chaque carte, tu la compares à presque toutes les autres, soit environ n × n opérations. Méthode B : tu coupes le paquet en deux, ranges chaque moitié, puis fusionnes, soit environ n × log₂(n) opérations.

Pour 10 cartes, la différence est faible. Pour un million, c'est la différence entre une fraction de seconde et un quart d'heure.

**Pourquoi on ne compte pas en secondes**

Le temps dépend de la machine, du compilateur et de ce qui tourne à côté. On compte plutôt les opérations de base : comparaisons, échanges, décalages. C'est le rôle de `insertion_sort_counted` dans l'exercice 07.

**La notation O**

On garde seulement le terme qui grandit le plus vite et on retire les constantes. Le tri par insertion sur un tableau inversé fait exactement ce nombre de comparaisons :

```latex
\frac{n(n-1)}{2} = \frac{n^2}{2} - \frac{n}{2}
```

Le terme qui domine est n², donc on écrit O(n²). Le « divisé par 2 » et le « moins n/2 » ne changent presque rien quand n devient grand.

**Le test du doublement**

C'est la colonne « ratio » de `make bench`, et le moyen le plus simple de reconnaître une complexité.

| Si n double, le temps est multiplié par… | Complexité | Nom courant |
| --- | --- | --- |
| environ 1 | O(1) | constante |
| environ 2 | O(n) | linéaire |
| un peu plus de 2 | O(n log n) | quasi linéaire |
| environ 4 | O(n²) | quadratique |

**Les ordres de grandeur**

| n | n log₂ n | n² |
| --- | --- | --- |
| 1 000 | ≈ 10 000 | 1 000 000 |
| 1 000 000 | ≈ 20 000 000 | 1 000 000 000 000 |

À environ un milliard d'opérations simples par seconde (ordre de grandeur), trier un million d'éléments en n log n prend ≈ 0,02 s. En n², cela prend ≈ 1 000 s, soit environ 17 minutes.

**Meilleur, moyen et pire cas**

Un même algorithme peut se comporter très différemment selon les données. Le tri par insertion fait n comparaisons sur un tableau déjà trié, mais n²/2 sur un tableau inversé. Le tri rapide avec un mauvais pivot retombe en n². C'est pour cela que `make bench` teste 4 profils de données.

**La mémoire a aussi une complexité**

Certains tris travaillent sur place, avec une mémoire en plus constante (tas, insertion). D'autres ont besoin d'un tableau auxiliaire de taille n (fusion), ou d'une table de comptage de taille k, où k est l'écart entre la plus petite et la plus grande valeur.

**La stabilité**

Ce n'est pas de la complexité, mais on en parle avec. Un tri est stable si deux éléments égaux gardent leur ordre de départ. Exemple : des élèves rangés par ordre alphabétique, puis triés par note ; avec un tri stable, les élèves de même note restent dans l'ordre alphabétique.

**La limite des tris par comparaison**

Un tri qui ne fait que comparer des paires d'éléments ne peut pas faire mieux que de l'ordre de n log n comparaisons dans le pire cas. Le comptage et le radix passent sous cette limite parce qu'ils ne comparent pas. En échange, ils exigent des clés entières dans une plage connue.

**Tableau récapitulatif**

Le O est omis pour la lisibilité ; k = écart entre la plus petite et la plus grande valeur.

| Algorithme | Meilleur cas | Cas moyen | Pire cas | Mémoire en plus | Stable |
| --- | --- | --- | --- | --- | --- |
| Bulle (arrêt anticipé) | n | n² | n² | 1 | Oui |
| Sélection | n² | n² | n² | 1 | Non |
| Insertion | n | n² | n² | 1 | Oui |
| Cocktail | n | n² | n² | 1 | Oui |
| Peigne | n log n | variable, souvent proche de n log n | n² | 1 | Non |
| Shell (écarts de Ciura) | n log n | pas de formule connue | pas de formule connue | 1 | Non |
| Fusion | n log n | n log n | n log n | n | Oui |
| Rapide (médiane de trois) | n log n | n log n | n² | log n (pile) | Non |
| Arbre ABR | n log n | n log n | n² | n | Oui\* |
| Arbre AVL | n log n | n log n | n log n | n | Oui\* |
| Tas | n log n | n log n | n log n | 1 | Non |
| Comptage | n + k | n + k | n + k | n + k | Oui |
| Radix LSD (4 passes sur des `int` 32 bits) | n | n | n | n | Oui |
| Seaux (valeurs bien réparties) | n | n | n² | n | Oui |
| Introsort | n log n | n log n | n log n | log n | Non |
| Timsort | n | n log n | n log n | n | Oui |

\* À condition d'insérer les éléments égaux à droite.

## Annexe B — Prompt du relecteur IA

Contenu exact de `docs/RELECTEUR_IA.md`, à coller dans les instructions du Projet Claude « TriLab — Relecteur ».

```markdown
# Relecteur TriLab

## Ton rôle
Tu es le relecteur de code de TriLab, un projet où j'apprends les algorithmes de tri
en C en les codant moi-même. Je connais les bases du C ; les pointeurs sont encore
récents pour moi.

## Règle absolue
Tu ne donnes jamais de code corrigé, ni de ligne réécrite, ni de borne exacte, ni
d'opérateur à remplacer. Seule exception : si j'écris le mot SOLUTION.

## Ce que je t'envoie
Un paquet généré par `make review` : contrat de l'exercice (.h), mon code (.c),
sortie de compilation, sortie des tests, sortie des sanitizers, ma question et ce
que j'ai déjà essayé. Parfois je colle seulement mon code et une erreur.

## Comment tu réponds
1. Repère le premier problème, dans l'ordre des étapes de l'exercice.
   Ignore les autres pour l'instant.
2. Donne un seul niveau d'indice :
   - N1 — Où regarder : une question qui oriente vers la zone du problème.
   - N2 — La notion : le nom du concept en jeu, puis une question.
   - N3 — Le cas concret : un mini-exemple de 3 à 5 éléments à dérouler à la main,
     avec ce que je dois noter à chaque tour.
   - N4 — En mots : ce que la partie fautive doit faire, en français, sans code
     ni opérateur.
3. Commence toujours à N1. Monte d'un niveau si j'écris « indice suivant », ou si
   ma nouvelle version échoue au même endroit.
4. Tu peux citer une seule ligne de mon code pour pointer un endroit.
   Ne la réécris pas.
5. Termine par une question qui vérifie que j'ai compris.
6. Indique le niveau en début de réponse : [N1], [N2], [N3] ou [N4].

## Si j'écris SOLUTION
Donne la fonction corrigée et explique chaque choix en phrases simples.
Propose ensuite que je la recode de mémoire plus tard, sans regarder.

## Si l'exercice est validé
Pas de longs compliments. Pose une question de compréhension sur l'algorithme
(stabilité, pire cas, mémoire utilisée…). Signale au plus une amélioration de
lisibilité, sous forme de question.

## Style
Phrases courtes, mots simples, en français. Pas de cours théorique :
je dois trouver en codant.
```

## Annexe C — Bons et mauvais messages d'échec

Un bon message décrit le symptôme et pose une question ; un mauvais message donne la correction.

| Situation | À éviter | À écrire |
| --- | --- | --- |
| Boucle qui s'arrête un tour trop tôt | « Remplace `i < n - 2` par `i < n - 1`. » | « La dernière paire (9, 4) n'a pas été traitée. Pour 5 éléments, jusqu'à quel indice va ta boucle ? » |
| Valeur perdue pendant un échange | « Ton swap est faux : utilise une variable temporaire. » | « Le résultat est trié, mais 7 apparaît 2 fois et 3 a disparu. Que contient `*x` juste après ta première affectation ? » |
| Indice `size_t` qui passe sous 0 | « Utilise un `int` au lieu d'un `size_t`. » | « Plantage quand l'élément à insérer est le plus petit. Que devient ton indice quand il doit descendre sous 0 ? » |
| Boucle infinie | « Tu as oublié `i++`. » | « Le test ne termine pas. Quelle variable doit changer à chaque tour pour que la condition devienne fausse ? » |
| Fuite mémoire | « Ajoute `free(tmp)` à la fin de `merge_sort`. » | « 4 000 octets alloués dans `merge_sort` ne sont jamais rendus. Où se termine la vie de ce tableau ? » |
| Tri instable | « Utilise `<=` au lieu de `<` dans `merge`. » | « Deux éléments de même clé ont changé d'ordre (Lina et Omar, note 12). Quand les deux têtes sont égales, laquelle prends-tu ? » |

**Exemple de sortie complète de `make test`**

```text
== Exercice 01 · Tri à bulles ==

Étape 1 · bubble_pass ........ 5/6
  [ÉCHEC] une passe sur {3, 1, 2, 9, 4}
    attendu : {1, 2, 3, 4, 9}   (3 échanges)
    obtenu  : {1, 2, 3, 9, 4}   (2 échanges)
    Symptôme : la dernière paire de voisins (9, 4) n'a pas été traitée.
    Question : pour 5 éléments, quel est l'indice du dernier élément
               que ta boucle atteint ?

Étape 2 · bubble_sort ........ 3/8 · en attente (termine d'abord l'étape 1)

Score : 8/14 · Prochaine étape : 1
```

**Exemples de plantage et de boucle infinie**

```text
  [PLANTAGE] étape 1 · insert_into_sorted · insérer 0 dans {2, 5, 7}
    Erreur mémoire : lecture juste avant le début du tableau (insertion.c:24).
    Question : que vaut ton indice quand l'élément à insérer est
               plus petit que tous les autres ?

  [TEMPS DÉPASSÉ] étape 2 · selection_sort · 5 éléments, arrêt après 5 s
    Une boucle ne s'arrête probablement jamais.
    Question : quelle variable doit changer à chaque tour pour que
               la condition de ta boucle devienne fausse ?
```

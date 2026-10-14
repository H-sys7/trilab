# Projet final : libtri

Tu rassembles tes 16 algorithmes dans une bibliothèque statique à une seule API, puis tu
écris deux outils qui l'utilisent. Tu reprends ton propre code des exercices et tu
l'adaptes.

Le projet est terminé quand `make check` passe dans `final/` (puis `make memcheck`), que
`make bench` a tourné et que `RAPPORT.md` est rempli.

## Ce que tu écris

| Fichier | Contenu |
| --- | --- |
| `src/libtri.c` | l'API de `include/libtri.h` : vérifie les arguments, appelle le bon tri |
| `src/common.c` | `tri_swap`, l'échange de deux éléments de `size` octets |
| `src/<algo>.c` | un tri par fichier, version générique (`void *`, `size`, `cmp`) ; comptage et radix en version `int` ; seaux en version `double` |
| `cli/tri.c` | l'outil `tri` (contrat ci-dessous) |
| `cli/tri_bench.c` | l'outil `tri-bench` (contrat ci-dessous) |
| `RAPPORT.md` | ton rapport final |

Ne modifie pas `include/`, `src/internal.h`, `tests/` ni `Makefile` : `make check` le
vérifie. Tes fonctions d'aide sont `static`.

## L'outil `tri`

```text
Usage : tri [-a ALGO] [-r] [-s] [-l] [-h] [FICHIER]
```

- Lit un entier par ligne dans `FICHIER`, ou sur l'entrée standard s'il n'y a pas de
  fichier. Écrit les entiers triés, un par ligne, sur la sortie standard.
- `-a ALGO` : algorithme, par son nom court (`tri_algo_name`). Par défaut : `timsort`.
- `-r` : ordre décroissant.
- `-s` : refuse un algorithme instable.
- `-l` : liste les algorithmes, un par ligne, sous la forme `nom stable` ou
  `nom instable`, dans l'ordre de l'énumération, puis s'arrête.
- `-h` : affiche une aide qui commence par `Usage`, puis s'arrête.
- Les options sont séparées (pas de `-rs`) et peuvent venir dans n'importe quel ordre.
  L'analyse de `argv` se fait à la main.
- Une ligne contient un entier décimal, éventuellement signé, entre `INT_MIN` et
  `INT_MAX`, avec des espaces ou tabulations autour si on veut. Une ligne vide est une
  erreur. La fin du fichier ne compte pas comme une ligne vide.
- Les messages d'erreur vont sur la sortie d'erreur et commencent par `tri: `.

| Code de sortie | Cas |
| --- | --- |
| 0 | succès (y compris `-l`, `-h` et une entrée vide) |
| 1 | fichier introuvable ou illisible |
| 2 | ligne invalide ; le message donne son numéro (`tri: ligne 3 : ...`), en comptant à partir de 1 |
| 3 | usage incorrect : option inconnue, `-a` sans nom, algorithme inconnu, `-s` avec un algorithme instable, plusieurs fichiers |
| 4 | mémoire insuffisante, ou tri refusé par libtri (par exemple le comptage sur une plage trop grande) |

## L'outil `tri-bench`

- Mesure tous les algorithmes de libtri (avec `tri_sort_int`) et `qsort` de la libc.
- Cinq profils : aléatoire, trié, inversé, presque trié, beaucoup de doublons (valeurs
  entre 0 et 99).
- Mêmes règles que `make bench` : `n` part de 1 000 et double ; un algorithme arrête de
  grandir dès qu'une mesure dépasse 2 s (et au plus à 4 194 304 éléments) ; chaque point
  est la médiane de 5 mesures ; le terminal affiche `n`, le temps et le ratio.
- Un algorithme que libtri refuse pour un profil (le comptage sur une plage trop grande)
  est signalé, sans arrêter le banc.
- Export CSV dans `build/tri_bench.csv`, au format de l'exercice 07 :
  `algo,profil,n,temps_ms,ratio`.

## Commandes (dans `final/`)

| Commande | Effet |
| --- | --- |
| `make` | compile la bibliothèque, les tests et les deux outils |
| `make test` | tests de libtri, puis de l'outil `tri` |
| `make check` | intégrité, ASan + UBSan, tous les tests : critère de validation |
| `make memcheck` | les mêmes tests sous Valgrind |
| `make bench` | lance `tri-bench` (compilé en `-O2`) |
| `make debug T=<test>` | gdb sur un test de libtri |
| `make clean` | supprime `build/` |

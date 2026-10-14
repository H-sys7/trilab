/* Exercice 07 — Mesure et complexité. NE PAS MODIFIER CE FICHIER. */
#ifndef MESURE_H
#define MESURE_H

#include <stddef.h>

/* Un tri d'entiers, passé en paramètre comme n'importe quelle valeur. */
typedef void (*sort_fn)(int *, size_t);

/* Compteurs d'opérations d'un tri. */
struct op_stats {
    size_t comparisons; /* comparaisons entre deux éléments du tableau */
    size_t shifts;      /* décalages d'un élément d'une case vers la droite */
};

/* Une ligne du fichier CSV du banc de mesure. */
struct bench_row {
    const char *algo;    /* nom court du tri, ex. "bulle" */
    const char *profile; /* profil des données, ex. "aleatoire" */
    size_t n;            /* nombre d'éléments */
    double time_ms;      /* temps médian en millisecondes */
    double ratio;        /* temps(n) / temps(n / 2), ou 0 si inconnu */
};

/* Étape 1. Remplit a[0..n) avec des entiers pseudo-aléatoires de [0, max] : appelle
 * srand(seed), puis tire chaque valeur avec rand(). Même graine = même tableau.
 * Précondition : 0 <= max < RAND_MAX. a peut valoir NULL si n == 0. */
void fill_random(int *a, size_t n, unsigned seed, int max);

/* Étape 2. Remplit a[0..n) avec 0, 1, 2, ..., n - 1. */
void fill_sorted(int *a, size_t n);

/* Étape 3. Remplit a[0..n) avec n - 1, n - 2, ..., 1, 0. */
void fill_reversed(int *a, size_t n);

/* Étape 4. Remplit a[0..n) avec 0, 1, ..., n - 1, puis fait k échanges de deux
 * voisins a[j] et a[j + 1], avec k = n / 100 (au moins 1 si n >= 2) et chaque j
 * tiré au hasard : srand(seed), puis rand(). */
void fill_nearly_sorted(int *a, size_t n, unsigned seed);

/* Étape 5. Appelle sort(a, n) une fois et renvoie la durée de cet appel en
 * millisecondes, mesurée avec clock(). */
double time_sort(sort_fn sort, int *a, size_t n);

/* Étape 6. Trie a[0..n) comme le tri par insertion de l'exercice 03 et compte
 * dans *stats les comparaisons entre éléments et les décalages. Les compteurs
 * sont remis à zéro au début. a peut valoir NULL si n == 0. */
void insertion_sort_counted(int *a, size_t n, struct op_stats *stats);

/* Étape 7. Écrit dans le fichier path une ligne d'en-tête
 * « algo,profil,n,temps_ms,ratio », puis une ligne par élément de rows :
 * temps avec 3 décimales, ratio avec 2 décimales, ratio laissé vide s'il vaut 0.
 * Exemple : « bulle,aleatoire,2000,3.300,3.87 ». Le fichier est fermé avant le
 * retour. Renvoie 0, ou -1 si le fichier ne peut pas être ouvert. */
int bench_write_csv(const char *path, const struct bench_row *rows, size_t count);

#endif

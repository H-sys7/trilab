/* Exercice 09 — Tri rapide. NE PAS MODIFIER CE FICHIER. */
#ifndef QUICK_H
#define QUICK_H

#include <stddef.h>

/* Étape 1. Partition de a[lo..hi) autour du pivot v = a[hi - 1] (sa valeur avant
 * l'appel). Réorganise la zone et renvoie p tel que : a[p] == v, toutes les valeurs
 * de a[lo..p) sont < v, toutes celles de a[p + 1..hi) sont >= v. Les cases hors de
 * a[lo..hi) ne changent pas. Précondition : lo < hi. */
size_t partition_last(int *a, size_t lo, size_t hi);

/* Étape 2. Trie a[0..n) par ordre croissant : partition_last sur la zone, puis tri
 * récursif des deux parties de part et d'autre du pivot. a peut valoir NULL si
 * n == 0. */
void quick_sort_naive(int *a, size_t n);

/* Étape 3. Avec mid = lo + (hi - lo) / 2 : place en a[hi - 1] la médiane des trois
 * valeurs a[lo], a[mid] et a[hi - 1]. Ces trois cases gardent ces trois valeurs
 * (dans un ordre quelconque) ; les autres cases ne changent pas.
 * Précondition : hi - lo >= 3. */
void median_of_three(int *a, size_t lo, size_t hi);

/* Étape 4. Trie a[0..n) par ordre croissant comme quick_sort_naive, mais toute zone
 * d'au moins 3 éléments choisit d'abord son pivot avec median_of_three. Reste
 * rapide sur un tableau déjà trié ou trié à l'envers. a peut valoir NULL si n == 0. */
void quick_sort(int *a, size_t n);

/* Bonus 1 (make test BONUS=1). Partition de Hoare de a[lo..hi) autour du pivot
 * v = a[lo] (sa valeur avant l'appel). Renvoie j, avec lo <= j < hi - 1, tel que
 * toutes les valeurs de a[lo..j] sont <= v et toutes celles de a[j + 1..hi) sont
 * >= v. Précondition : hi - lo >= 2. */
size_t partition_hoare(int *a, size_t lo, size_t hi);

/* Bonus 2. Partition à trois voies de a[lo..hi) autour du pivot v = a[lo] (sa valeur
 * avant l'appel). Après l'appel : a[lo..*lt) < v, a[*lt..*gt) == v, a[*gt..hi) > v.
 * Précondition : lo < hi. */
void partition_three_way(int *a, size_t lo, size_t hi, size_t *lt, size_t *gt);

/* Bonus 3. Trie a[0..n) avec partition_three_way. Reste rapide même quand le tableau
 * contient énormément de valeurs égales. a peut valoir NULL si n == 0. */
void quick_sort_3way(int *a, size_t n);

#endif

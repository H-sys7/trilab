/* Exercice 13 — Tri par comptage. NE PAS MODIFIER CE FICHIER. */
#ifndef COUNTING_H
#define COUNTING_H

#include <stddef.h>

/* Nombre maximal de cases de la table de comptage (k = max - min + 1). */
#define COUNTING_MAX_RANGE 10000000

/* Étape 1. Écrit dans *min et *max la plus petite et la plus grande valeur de
 * a[0..n). Précondition : n >= 1. */
void find_min_max(const int *a, size_t n, int *min, int *max);

/* Étape 2. count[0..k) contient des zéros, avec k = max - min + 1 où max est la plus
 * grande valeur de a. Pour chaque valeur v de a[0..n), augmente count[v - min] de 1. */
void count_occurrences(const int *a, size_t n, int min, size_t *count);

/* Étape 3. Remplace chaque count[i] par la somme des cases qui le précèdent :
 * count[0] + ... + count[i - 1] (0 pour la première case). count[i] devient ainsi
 * la position, dans le tableau trié, de la première valeur min + i. */
void prefix_sums(size_t *count, size_t k);

/* Étape 4. Trie a[0..n) par ordre croissant en comptant les valeurs. La table de
 * comptage est allouée avec calloc et rendue avant le retour, comme toute autre
 * mémoire allouée. Renvoie 0 ; renvoie -1, sans modifier a, si k = max - min + 1
 * dépasse COUNTING_MAX_RANGE ou si une allocation échoue. a peut valoir NULL si
 * n == 0. */
int counting_sort(int *a, size_t n);

#endif

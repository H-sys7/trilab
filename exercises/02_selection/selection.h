/* Exercice 02 — Tri par sélection. NE PAS MODIFIER CE FICHIER. */
#ifndef SELECTION_H
#define SELECTION_H

#include <stddef.h>

/* Étape 1. Renvoie l'indice de la plus petite valeur de a[from..n).
 * En cas d'égalité, renvoie le plus petit de ces indices.
 * Ne modifie pas le tableau. Précondition : from < n. */
size_t min_index_from(const int *a, size_t from, size_t n);

/* Étape 2. Trie a[0..n) par ordre croissant, en plaçant à chaque tour la plus
 * petite valeur restante à sa place définitive. a peut valoir NULL si n == 0. */
void selection_sort(int *a, size_t n);

#endif

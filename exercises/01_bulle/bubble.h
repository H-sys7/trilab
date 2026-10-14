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

/* Bonus 1 (make test BONUS=1). Même passe que bubble_pass, mais renvoie la position
 * du dernier échange : l'indice de la case qui a reçu le plus grand des deux éléments
 * échangés en dernier, ou 0 si aucun échange. Après l'appel, a[pos..n) est trié
 * et chacun de ses éléments est à sa place définitive. */
size_t bubble_pass_last(int *a, size_t n);

/* Bonus 2. Trie a[0..n) par ordre croissant, sans jamais repasser sur la partie
 * que bubble_pass_last a déclarée à sa place définitive. */
void bubble_sort_last(int *a, size_t n);

#endif

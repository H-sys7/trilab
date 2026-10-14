/* Exercice 04 — Tri cocktail. NE PAS MODIFIER CE FICHIER. */
#ifndef COCKTAIL_H
#define COCKTAIL_H

#include <stddef.h>

/* Étape 1. Passe de gauche à droite sur a[lo..hi) : chaque paire de voisins mal
 * ordonnée, rencontrée de gauche à droite, est échangée. Après l'appel, le plus
 * grand élément de a[lo..hi) est en a[hi - 1]. Les cases hors de a[lo..hi) ne
 * changent pas. Renvoie le nombre d'échanges. Précondition : lo < hi. */
size_t cocktail_forward(int *a, size_t lo, size_t hi);

/* Étape 2. Passe de droite à gauche sur a[lo..hi) : chaque paire de voisins mal
 * ordonnée, rencontrée de droite à gauche, est échangée. Après l'appel, le plus
 * petit élément de a[lo..hi) est en a[lo]. Les cases hors de a[lo..hi) ne
 * changent pas. Renvoie le nombre d'échanges. Précondition : lo < hi. */
size_t cocktail_backward(int *a, size_t lo, size_t hi);

/* Étape 3. Trie a[0..n) par ordre croissant en alternant les deux passes sur une
 * zone qui rétrécit des deux côtés. S'arrête dès qu'une passe ne fait aucun
 * échange. a peut valoir NULL si n == 0. */
void cocktail_sort(int *a, size_t n);

#endif

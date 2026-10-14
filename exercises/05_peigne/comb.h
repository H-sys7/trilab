/* Exercice 05 — Tri à peigne. NE PAS MODIFIER CE FICHIER. */
#ifndef COMB_H
#define COMB_H

#include <stddef.h>

/* Étape 1. Renvoie l'écart suivant : gap divisé par 1,3, arrondi vers le bas,
 * et jamais moins que 1. Précondition : gap >= 1. */
size_t comb_next_gap(size_t gap);

/* Étape 2. Une passe d'écart gap sur a[0..n) : chaque paire (a[i], a[i + gap])
 * mal ordonnée, rencontrée de gauche à droite, est échangée. Avec gap == 1,
 * c'est une passe du tri à bulles. Renvoie le nombre d'échanges.
 * Précondition : gap >= 1. a peut valoir NULL si n == 0. */
size_t comb_pass(int *a, size_t n, size_t gap);

/* Étape 3. Trie a[0..n) par ordre croissant. L'écart part de n et diminue avec
 * comb_next_gap ; une fois arrivé à 1, les passes continuent jusqu'à ce qu'une
 * passe ne fasse aucun échange. a peut valoir NULL si n == 0. */
void comb_sort(int *a, size_t n);

#endif

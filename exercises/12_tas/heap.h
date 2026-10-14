/* Exercice 12 — Tri par tas. NE PAS MODIFIER CE FICHIER. */
#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>

/* Un tableau a[0..n) se lit comme un arbre binaire : les enfants de la case i sont
 * les cases 2i + 1 et 2i + 2 (quand elles sont < n). C'est un tas max si chaque case
 * est >= à ses enfants. */

/* Étape 1. Les sous-arbres des enfants de i sont déjà des tas max. Fait descendre
 * a[i] en l'échangeant avec le plus grand de ses enfants (le gauche en cas
 * d'égalité), tant qu'il est plus petit que lui. Après l'appel, le sous-arbre de i
 * est un tas max. Seules les cases de ce sous-arbre changent ; les cases >= n ne
 * sont jamais lues. Précondition : i < n. */
void sift_down(int *a, size_t n, size_t i);

/* Étape 2. Transforme a[0..n) en tas max, sur place, en un temps proportionnel à n.
 * a peut valoir NULL si n == 0. */
void build_max_heap(int *a, size_t n);

/* Étape 3. Trie a[0..n) par ordre croissant, sur place : construit un tas max, puis,
 * tant que la zone du tas contient au moins 2 cases, échange sa racine (le maximum)
 * avec sa dernière case, réduit la zone d'une case et rétablit le tas.
 * a peut valoir NULL si n == 0. */
void heap_sort(int *a, size_t n);

#endif

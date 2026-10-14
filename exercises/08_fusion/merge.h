/* Exercice 08 — Tri fusion. NE PAS MODIFIER CE FICHIER. */
#ifndef MERGE_H
#define MERGE_H

#include <stddef.h>

/* Étape 1. a[lo..mid) et a[mid..hi) sont déjà triés. Après l'appel, a[lo..hi) est
 * trié et contient les mêmes valeurs. tmp[lo..hi) sert d'espace de travail ; les
 * autres cases de a et de tmp ne changent pas. À égalité, l'élément de la moitié
 * gauche passe en premier (fusion stable). Précondition : lo <= mid <= hi. */
void merge(int *a, int *tmp, size_t lo, size_t mid, size_t hi);

/* Étape 2. Trie a[lo..hi) : coupe la zone en deux moitiés, trie chacune par un
 * appel récursif, puis les fusionne avec merge. tmp[lo..hi) sert d'espace de
 * travail. N'alloue aucune mémoire. Précondition : lo <= hi. */
void merge_sort_rec(int *a, int *tmp, size_t lo, size_t hi);

/* Étape 3. Trie a[0..n) par ordre croissant. Une seule allocation (le tableau de
 * travail), rendue avant le retour. Si l'allocation échoue, a n'est pas modifié.
 * a peut valoir NULL si n == 0. */
void merge_sort(int *a, size_t n);

/* Bonus (make test BONUS=1). Trie a[0..n) sans récursion : fusionne des blocs de
 * taille 1, puis 2, puis 4, et ainsi de suite. Une seule allocation, rendue avant
 * le retour. a peut valoir NULL si n == 0. */
void merge_sort_bottom_up(int *a, size_t n);

#endif

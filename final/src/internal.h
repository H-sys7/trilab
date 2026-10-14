/* TriLab — libtri : fonctions internes, une par fichier de src/. NE PAS MODIFIER CE FICHIER.
 * Chaque tri générique reçoit des arguments déjà vérifiés par libtri.c : base non NULL
 * (sauf si n == 0), size >= 1, cmp non NULL. Ils renvoient TRI_OK, ou TRI_ENOMEM si une
 * allocation échoue (le tableau est alors inchangé). Tes fonctions d'aide sont static.
 */
#ifndef TRI_INTERNAL_H
#define TRI_INTERNAL_H

#include "libtri.h"

/* common.c : échange les size octets pointés par x et y (x et y peuvent être égaux). */
void tri_swap(void *x, void *y, size_t size);

/* Un fichier par algorithme : bubble.c, selection.c, insertion.c, ... */
tri_status tri_bubble(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_selection(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_insertion(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_cocktail(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_comb(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_shell(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_merge(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_quick(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_tree(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_avl(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_heap(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_introsort(void *base, size_t n, size_t size, tri_cmp cmp);
tri_status tri_timsort(void *base, size_t n, size_t size, tri_cmp cmp);

/* Tris d'entiers sans comparaison (a non NULL sauf si n == 0). Le comptage renvoie
 * TRI_EUNSUPPORTED si k = max - min + 1 dépasse TRI_COUNTING_MAX_RANGE. */
tri_status tri_counting_int(int *a, size_t n);
tri_status tri_radix_int(int *a, size_t n);

/* bucket.c : valeurs déjà vérifiées dans [0, 1). */
tri_status tri_bucket(double *a, size_t n);

#endif

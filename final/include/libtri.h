/* TriLab — libtri : API publique unique. NE PAS MODIFIER CE FICHIER. */
#ifndef LIBTRI_H
#define LIBTRI_H

#include <stdbool.h>
#include <stddef.h>

/* Comparateur façon qsort : < 0, 0 ou > 0. */
typedef int (*tri_cmp)(const void *, const void *);

/* Les algorithmes. Leurs noms (tri_algo_name), dans l'ordre :
 * bulle, selection, insertion, cocktail, peigne, shell, fusion, rapide, abr, avl,
 * tas, comptage, radix, introsort, timsort. */
typedef enum {
    TRI_BUBBLE, TRI_SELECTION, TRI_INSERTION, TRI_COCKTAIL, TRI_COMB, TRI_SHELL,
    TRI_MERGE, TRI_QUICK, TRI_TREE, TRI_AVL, TRI_HEAP,
    TRI_COUNTING, TRI_RADIX, TRI_INTROSORT, TRI_TIMSORT,
    TRI_ALGO_COUNT
} tri_algo;

/* TRI_OK : succès. TRI_EINVAL : argument invalide (le tableau n'est pas modifié).
 * TRI_ENOMEM : allocation impossible (le tableau n'est pas modifié).
 * TRI_EUNSUPPORTED : combinaison non prise en charge (le tableau n'est pas modifié). */
typedef enum { TRI_OK = 0, TRI_EINVAL, TRI_ENOMEM, TRI_EUNSUPPORTED } tri_status;

/* Écart maximal accepté par le tri par comptage : k = max - min + 1 cases. */
#define TRI_COUNTING_MAX_RANGE 10000000

/* Tri générique façon qsort de base[0..n), éléments de size octets, ordre donné par cmp.
 * Comptage et radix renvoient TRI_EUNSUPPORTED. TRI_EINVAL si base == NULL avec n > 0,
 * si size == 0, si cmp == NULL ou si algo n'est pas un algorithme. n == 0 : TRI_OK.
 * Les algorithmes déclarés stables par tri_algo_is_stable doivent l'être vraiment. */
tri_status tri_sort(void *base, size_t n, size_t size, tri_cmp cmp, tri_algo algo);

/* Tri d'entiers : accepte tous les algorithmes, y compris comptage et radix.
 * TRI_EINVAL si a == NULL avec n > 0, ou si algo n'est pas un algorithme.
 * Le comptage renvoie TRI_EUNSUPPORTED si k dépasse TRI_COUNTING_MAX_RANGE. */
tri_status tri_sort_int(int *a, size_t n, tri_algo algo);

/* Tri par seaux : valeurs dans [0, 1). TRI_EINVAL si a == NULL avec n > 0, ou si une
 * valeur est hors de [0, 1). */
tri_status tri_sort_unit_doubles(double *a, size_t n);

/* Nom court de l'algorithme (voir la liste ci-dessus), ou NULL si algo n'en est pas un. */
const char *tri_algo_name(tri_algo algo);

/* true si l'algorithme est stable (deux éléments équivalents gardent leur ordre de
 * départ) : bulle, insertion, cocktail, fusion, abr, avl, comptage, radix, timsort.
 * false pour les autres, et pour une valeur qui n'est pas un algorithme. */
bool tri_algo_is_stable(tri_algo algo);

#endif

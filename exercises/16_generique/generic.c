/* Exercice 16 — Tri générique façon qsort
 * Notion C : void *, octets (unsigned char *), memcpy, pointeur de fonction comparateur.
 * Étapes : 1) generic_swap 2) cmp_int 3) cmp_double 4) cmp_student
 *          5) insertion_sort_generic 6) merge_sort_generic
 * Commandes : make test · make check · make memcheck · make review
 */
#include "generic.h"

#include <stdlib.h>
#include <string.h>

void generic_swap(void *x, void *y, size_t size)
{
    /* TODO(étape 1) */
    (void)x;
    (void)y;
    (void)size;
}

int cmp_int(const void *x, const void *y)
{
    /* TODO(étape 2) */
    (void)x;
    (void)y;
    return 0;
}

int cmp_double(const void *x, const void *y)
{
    /* TODO(étape 3) */
    (void)x;
    (void)y;
    return 0;
}

int cmp_student(const void *x, const void *y)
{
    /* TODO(étape 4) */
    (void)x;
    (void)y;
    return 0;
}

void insertion_sort_generic(void *base, size_t n, size_t size, cmp_fn cmp)
{
    /* TODO(étape 5) */
    (void)base;
    (void)n;
    (void)size;
    (void)cmp;
}

int merge_sort_generic(void *base, size_t n, size_t size, cmp_fn cmp)
{
    /* TODO(étape 6) */
    (void)base;
    (void)n;
    (void)size;
    (void)cmp;
    return 0;
}

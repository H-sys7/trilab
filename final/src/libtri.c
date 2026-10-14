/* libtri — l'API publique : vérifie les arguments, puis appelle le bon tri
 * Notion C : compilation séparée, bibliothèque statique, une API stable devant le code.
 * Étapes : voir les étapes de make test ; le contrat est dans include/libtri.h.
 * Commandes (dans final/) : make test · make check · make memcheck
 */
#include "internal.h"

tri_status tri_sort(void *base, size_t n, size_t size, tri_cmp cmp, tri_algo algo)
{
    /* TODO */
    (void)base;
    (void)n;
    (void)size;
    (void)cmp;
    (void)algo;
    return TRI_OK;
}

tri_status tri_sort_int(int *a, size_t n, tri_algo algo)
{
    /* TODO */
    (void)a;
    (void)n;
    (void)algo;
    return TRI_OK;
}

tri_status tri_sort_unit_doubles(double *a, size_t n)
{
    /* TODO */
    (void)a;
    (void)n;
    return TRI_OK;
}

const char *tri_algo_name(tri_algo algo)
{
    /* TODO */
    (void)algo;
    return NULL;
}

bool tri_algo_is_stable(tri_algo algo)
{
    /* TODO */
    (void)algo;
    return false;
}

/* Exercice 09 — Tri rapide
 * Notion C : la récursion sur deux sous-zones, la partition, le choix du pivot.
 * Étapes : 1) partition_last 2) quick_sort_naive 3) median_of_three 4) quick_sort
 *          (bonus : partition_hoare, partition_three_way, quick_sort_3way)
 * Commandes : make test · make check · make memcheck · make review
 */
#include "quick.h"

/* TODO(étape 1) : échange les valeurs pointées par x et y. */
static void swap(int *x, int *y)
{
    (void)x;
    (void)y;
}

size_t partition_last(int *a, size_t lo, size_t hi)
{
    /* TODO(étape 1) */
    (void)swap;
    (void)a;
    (void)lo;
    (void)hi;
    return 0;
}

void quick_sort_naive(int *a, size_t n)
{
    /* TODO(étape 2) */
    (void)a;
    (void)n;
}

void median_of_three(int *a, size_t lo, size_t hi)
{
    /* TODO(étape 3) */
    (void)a;
    (void)lo;
    (void)hi;
}

void quick_sort(int *a, size_t n)
{
    /* TODO(étape 4) */
    (void)a;
    (void)n;
}

size_t partition_hoare(int *a, size_t lo, size_t hi)
{
    /* TODO(bonus 1) */
    (void)a;
    (void)lo;
    (void)hi;
    return 0;
}

void partition_three_way(int *a, size_t lo, size_t hi, size_t *lt, size_t *gt)
{
    /* TODO(bonus 2) */
    (void)a;
    (void)lo;
    (void)hi;
    (void)lt;
    (void)gt;
}

void quick_sort_3way(int *a, size_t n)
{
    /* TODO(bonus 3) */
    (void)a;
    (void)n;
}

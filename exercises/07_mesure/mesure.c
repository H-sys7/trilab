/* Exercice 07 — Mesure et complexité
 * Notion C : pointeur de fonction, première struct, clock() et FILE *.
 * Étapes : 1) fill_random 2) fill_sorted 3) fill_reversed 4) fill_nearly_sorted
 *          5) time_sort 6) insertion_sort_counted 7) bench_write_csv
 * Commandes : make test · make check · make review · puis make bench (racine)
 */
#include "mesure.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fill_random(int *a, size_t n, unsigned seed, int max)
{
    /* TODO(étape 1) */
    (void)a;
    (void)n;
    (void)seed;
    (void)max;
}

void fill_sorted(int *a, size_t n)
{
    /* TODO(étape 2) */
    (void)a;
    (void)n;
}

void fill_reversed(int *a, size_t n)
{
    /* TODO(étape 3) */
    (void)a;
    (void)n;
}

void fill_nearly_sorted(int *a, size_t n, unsigned seed)
{
    /* TODO(étape 4) */
    (void)a;
    (void)n;
    (void)seed;
}

double time_sort(sort_fn sort, int *a, size_t n)
{
    /* TODO(étape 5) */
    (void)sort;
    (void)a;
    (void)n;
    return 0.0;
}

void insertion_sort_counted(int *a, size_t n, struct op_stats *stats)
{
    /* TODO(étape 6) */
    (void)a;
    (void)n;
    (void)stats;
}

int bench_write_csv(const char *path, const struct bench_row *rows, size_t count)
{
    /* TODO(étape 7) */
    (void)path;
    (void)rows;
    (void)count;
    return 0;
}

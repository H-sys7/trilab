/* Exercice 06 — Tri de Shell
 * Notion C : un tableau static const, partagé par le fichier et en lecture seule.
 * Étapes : 1) gapped_insertion   2) shell_sort
 * Commandes : make test · make check · make review
 */
#include "shell.h"

/* Écarts de Ciura (de 1 à 701), prolongés en multipliant par 2,25 environ. Fourni. */
static const size_t GAPS[] = { 510774, 227011, 100894, 44842, 19930, 8858, 3937, 1750,
                               701, 301, 132, 57, 23, 10, 4, 1 };

void gapped_insertion(int *a, size_t n, size_t gap)
{
    /* TODO(étape 1) */
    (void)a;
    (void)n;
    (void)gap;
}

void shell_sort(int *a, size_t n)
{
    /* TODO(étape 2) */
    (void)GAPS;
    (void)a;
    (void)n;
}

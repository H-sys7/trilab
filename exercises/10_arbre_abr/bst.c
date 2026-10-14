/* Exercice 10 — Tri par arbre binaire de recherche
 * Notion C : une struct récursive, un pointeur vers pointeur, une libération récursive.
 * Étapes : 1) bst_insert 2) bst_to_array 3) bst_free 4) tree_sort (bonus : bst_insert_iter)
 * Commandes : make test · make check · make memcheck · make review
 */
#include "bst.h"

#include <stdlib.h>

int bst_insert(struct bst_node **root, int key)
{
    /* TODO(étape 1) */
    (void)root;
    (void)key;
    return 0;
}

size_t bst_to_array(const struct bst_node *root, int *out)
{
    /* TODO(étape 2) */
    (void)root;
    (void)out;
    return 0;
}

void bst_free(struct bst_node *root)
{
    /* TODO(étape 3) */
    (void)root;
}

void tree_sort(int *a, size_t n)
{
    /* TODO(étape 4) */
    (void)a;
    (void)n;
}

int bst_insert_iter(struct bst_node **root, int key)
{
    /* TODO(bonus) */
    (void)root;
    (void)key;
    return 0;
}

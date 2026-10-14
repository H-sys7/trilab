/* Exercice 11 — Tri par arbre AVL
 * Notion C : la hauteur d'un nœud, les rotations, une fonction qui renvoie la nouvelle racine.
 * Étapes : 1) avl_height 2) avl_rotate_left 3) avl_rotate_right 4) avl_insert 5) avl_sort
 * Commandes : make test · make check · make memcheck · make review
 */
#include "avl.h"

#include <stdlib.h>

int avl_height(const struct avl_node *node)
{
    /* TODO(étape 1) */
    (void)node;
    return 0;
}

struct avl_node *avl_rotate_left(struct avl_node *x)
{
    /* TODO(étape 2) */
    return x;
}

struct avl_node *avl_rotate_right(struct avl_node *y)
{
    /* TODO(étape 3) */
    return y;
}

struct avl_node *avl_insert(struct avl_node *root, int key)
{
    /* TODO(étape 4) */
    (void)key;
    return root;
}

void avl_sort(int *a, size_t n)
{
    /* TODO(étape 5) */
    (void)a;
    (void)n;
}

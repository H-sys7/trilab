/* Exercice 11 — Tri par arbre AVL. NE PAS MODIFIER CE FICHIER. */
#ifndef AVL_H
#define AVL_H

#include <stddef.h>

/* Un nœud d'arbre AVL : un ABR où, en chaque nœud, les hauteurs des deux
 * sous-arbres diffèrent d'au plus 1. */
struct avl_node {
    int key;
    int height; /* hauteur du sous-arbre de ce nœud : 1 pour une feuille */
    struct avl_node *left;
    struct avl_node *right;
};

/* Étape 1. Renvoie la hauteur stockée dans node, ou 0 si node vaut NULL. */
int avl_height(const struct avl_node *node);

/* Étape 2. Rotation à gauche autour de x (x->right n'est pas NULL) : l'ancien fils
 * droit de x devient la racine du sous-arbre, x devient son fils gauche. L'ordre
 * infixe des clés ne change pas. Met à jour la hauteur des deux nœuds déplacés
 * (1 + la plus grande hauteur de leurs enfants). Renvoie la nouvelle racine. */
struct avl_node *avl_rotate_left(struct avl_node *x);

/* Étape 3. Rotation à droite autour de y (y->left n'est pas NULL), symétrique de la
 * rotation à gauche. Renvoie la nouvelle racine. */
struct avl_node *avl_rotate_right(struct avl_node *y);

/* Étape 4. Insère key dans l'AVL de racine root (NULL = arbre vide) : nouvelle
 * feuille allouée avec malloc, clés égales à droite, puis rééquilibrage par rotations
 * sur le chemin du retour. Renvoie la nouvelle racine. Si l'allocation échoue,
 * l'arbre est renvoyé inchangé. */
struct avl_node *avl_insert(struct avl_node *root, int key);

/* Étape 5. Trie a[0..n) par ordre croissant avec un AVL, puis rend tous ses nœuds.
 * a peut valoir NULL si n == 0. */
void avl_sort(int *a, size_t n);

#endif

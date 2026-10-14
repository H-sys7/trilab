/* Exercice 10 — Tri par arbre binaire de recherche. NE PAS MODIFIER CE FICHIER. */
#ifndef BST_H
#define BST_H

#include <stddef.h>

/* Un nœud d'arbre binaire de recherche (ABR). */
struct bst_node {
    int key;
    struct bst_node *left;  /* clés plus petites que key */
    struct bst_node *right; /* clés plus grandes ou égales à key */
};

/* Étape 1. Insère key dans l'ABR dont la racine est *root (NULL = arbre vide) : un
 * nouveau nœud, alloué avec malloc, est accroché à sa place sous forme de feuille.
 * Une clé égale à celle d'un nœud va dans son sous-arbre droit.
 * Renvoie 0, ou -1 si l'allocation échoue (l'arbre est alors inchangé). */
int bst_insert(struct bst_node **root, int key);

/* Étape 2. Écrit les clés de l'arbre dans out[0], out[1], ... dans l'ordre d'un
 * parcours infixe (sous-arbre gauche, nœud, sous-arbre droit). Renvoie le nombre de
 * clés écrites. root peut valoir NULL. */
size_t bst_to_array(const struct bst_node *root, int *out);

/* Étape 3. Rend avec free tous les nœuds de l'arbre. root peut valoir NULL. */
void bst_free(struct bst_node *root);

/* Étape 4. Trie a[0..n) par ordre croissant : insère chaque valeur dans un ABR,
 * recopie l'arbre dans a, puis rend l'arbre. a peut valoir NULL si n == 0. */
void tree_sort(int *a, size_t n);

/* Bonus (make test BONUS=1). Même contrat que bst_insert, sans aucun appel récursif. */
int bst_insert_iter(struct bst_node **root, int key);

#endif

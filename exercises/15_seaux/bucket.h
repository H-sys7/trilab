/* Exercice 15 — Tri par seaux. NE PAS MODIFIER CE FICHIER. */
#ifndef BUCKET_H
#define BUCKET_H

#include <stddef.h>

/* Un seau : un tableau dynamique de double. */
struct bucket {
    double *items; /* zone allouée (NULL si cap == 0) */
    size_t len;    /* nombre de valeurs rangées */
    size_t cap;    /* nombre de cases allouées */
};

/* Étape 1. Renvoie le numéro du seau de x parmi k seaux : la partie entière de
 * x × k. Le résultat est toujours < k. Précondition : 0 <= x < 1 et k >= 1. */
size_t bucket_index(double x, size_t k);

/* Étape 2. Ajoute x à la fin du seau b. Quand il est plein (len == cap), agrandit
 * d'abord sa zone avec realloc : la capacité double, ou passe à 4 si elle valait 0.
 * Renvoie 0, ou -1 si realloc échoue (le seau est alors inchangé). */
int bucket_push(struct bucket *b, double x);

/* Étape 3. Trie a[0..n) par ordre croissant ; toutes les valeurs sont dans [0, 1).
 * Utilise n seaux : chaque valeur va dans son seau, chaque seau est trié par
 * insertion, puis les seaux sont recopiés dans a dans l'ordre. Toute la mémoire est
 * rendue avant le retour. Renvoie 0, ou -1 si une allocation échoue.
 * a peut valoir NULL si n == 0. */
int bucket_sort(double *a, size_t n);

#endif

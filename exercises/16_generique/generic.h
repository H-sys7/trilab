/* Exercice 16 — Tri générique façon qsort. NE PAS MODIFIER CE FICHIER. */
#ifndef GENERIC_H
#define GENERIC_H

#include <stddef.h>

/* Un comparateur façon qsort : renvoie un nombre < 0 si *x vient avant *y, 0 s'ils
 * sont équivalents, > 0 si *x vient après *y. */
typedef int (*cmp_fn)(const void *x, const void *y);

/* Un élève : un nom et une note. */
struct student {
    char name[16];
    int grade;
};

/* Étape 1. Échange les size octets pointés par x et y (size quelconque, 0 compris).
 * x et y peuvent désigner la même zone. */
void generic_swap(void *x, void *y, size_t size);

/* Étape 2. Compare deux int pointés : ordre croissant. */
int cmp_int(const void *x, const void *y);

/* Étape 3. Compare deux double pointés : ordre croissant. */
int cmp_double(const void *x, const void *y);

/* Étape 4. Compare deux struct student pointées par note croissante. Les noms ne
 * comptent pas : deux élèves de même note sont équivalents. */
int cmp_student(const void *x, const void *y);

/* Étape 5. Trie base[0..n), n éléments de size octets, avec le comparateur cmp, par
 * insertion. Tri stable : deux éléments équivalents gardent leur ordre de départ.
 * base peut valoir NULL si n == 0. */
void insertion_sort_generic(void *base, size_t n, size_t size, cmp_fn cmp);

/* Étape 6. Comme insertion_sort_generic, mais par fusion (stable aussi). Une seule
 * allocation de n × size octets, rendue avant le retour. Renvoie 0, ou -1 si
 * l'allocation échoue (base est alors inchangé). */
int merge_sort_generic(void *base, size_t n, size_t size, cmp_fn cmp);

#endif

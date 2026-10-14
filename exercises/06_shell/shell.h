/* Exercice 06 — Tri de Shell. NE PAS MODIFIER CE FICHIER. */
#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

/* Étape 1. Tri par insertion avec écart gap : chaque sous-suite
 * a[r], a[r + gap], a[r + 2 gap], ... (pour r allant de 0 à gap - 1) est triée
 * par ordre croissant. Avec gap == 1, c'est un tri par insertion ordinaire.
 * Précondition : gap >= 1. a peut valoir NULL si n == 0. */
void gapped_insertion(int *a, size_t n, size_t gap);

/* Étape 2. Trie a[0..n) par ordre croissant en appliquant gapped_insertion
 * avec chaque écart de la suite fournie dans shell.c qui est plus petit que n,
 * du plus grand au plus petit. a peut valoir NULL si n == 0. */
void shell_sort(int *a, size_t n);

#endif

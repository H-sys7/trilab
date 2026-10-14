/* Exercice 03 — Tri par insertion. NE PAS MODIFIER CE FICHIER. */
#ifndef INSERTION_H
#define INSERTION_H

#include <stddef.h>

/* Étape 1. a[0..len) est déjà trié par ordre croissant. Range la valeur a[len]
 * à sa place : après l'appel, a[0..len] est trié et contient les mêmes valeurs.
 * Seules les cases a[0..len] peuvent changer. */
void insert_into_sorted(int *a, size_t len);

/* Étape 2. Trie a[0..n) par ordre croissant, en insérant les valeurs une à une
 * dans la partie déjà triée. Sur un tableau presque trié, le travail reste
 * proche de n. a peut valoir NULL si n == 0. */
void insertion_sort(int *a, size_t n);

#endif

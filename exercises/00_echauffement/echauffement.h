/* Exercice 00 — Échauffement : les pointeurs. NE PAS MODIFIER CE FICHIER. */
#ifndef ECHAUFFEMENT_H
#define ECHAUFFEMENT_H

#include <stdbool.h>
#include <stddef.h>

/* Étape 1. Échange les valeurs pointées par x et y : après l'appel, *x contient
 * l'ancienne valeur de *y, et *y l'ancienne valeur de *x.
 * x et y peuvent désigner la même case. */
void swap(int *x, int *y);

/* Étape 2. Renvoie l'indice de la plus petite valeur de a[0..n).
 * En cas d'égalité, renvoie le plus petit de ces indices. Précondition : n >= 1. */
size_t min_index(const int *a, size_t n);

/* Étape 3. Inverse l'ordre de a[0..n), sur place : l'ancien a[0] se retrouve
 * en a[n - 1], l'ancien a[1] en a[n - 2], et ainsi de suite.
 * a peut valoir NULL si n == 0. */
void reverse(int *a, size_t n);

/* Étape 4. Renvoie true si a[0..n) est trié par ordre croissant (égalités permises),
 * false sinon. Un tableau de 0 ou 1 élément est trié. a peut valoir NULL si n == 0. */
bool is_sorted(const int *a, size_t n);

#endif

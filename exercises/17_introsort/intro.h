/* Exercice 17 — Introsort. NE PAS MODIFIER CE FICHIER. */
#ifndef INTRO_H
#define INTRO_H

#include <stddef.h>

/* Taille en dessous de laquelle une zone est laissée au tri par insertion final. */
#define INTRO_SMALL 16

/* Étape 1. Renvoie la profondeur maximale de récursion : 2 × (partie entière de
 * log2(n)), ou 0 si n < 2. Exemples : n = 16 -> 8, n = 1000 -> 18. */
size_t depth_limit(size_t n);

/* Étape 2. Travail principal sur a[lo..hi), sans le finir : tant que la zone contient
 * plus de INTRO_SMALL éléments,
 *   - si depth vaut 0, la zone est triée entièrement par un tri par tas ;
 *   - sinon, elle est partitionnée (pivot : médiane de trois, comme à l'exercice 09),
 *     puis traitée de part et d'autre du pivot avec la profondeur depth - 1.
 * Les zones d'au plus INTRO_SMALL éléments sont laissées telles quelles.
 * Après l'appel : pour tous i < j de a[lo..hi) avec j - i >= INTRO_SMALL, a[i] <= a[j].
 * Les cases hors de a[lo..hi) ne changent pas. Précondition : lo <= hi. */
void introsort_loop(int *a, size_t lo, size_t hi, size_t depth);

/* Étape 3. Trie a[0..n) par ordre croissant : introsort_loop sur tout le tableau avec
 * depth_limit(n), puis un tri par insertion sur tout le tableau. Rapide même sur les
 * entrées qui font dégénérer le tri rapide. a peut valoir NULL si n == 0. */
void introsort(int *a, size_t n);

#endif

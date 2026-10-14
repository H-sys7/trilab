/* Exercice 18 — Timsort simplifié. NE PAS MODIFIER CE FICHIER. */
#ifndef TIM_H
#define TIM_H

#include <stddef.h>

/* Une suite (« run ») déjà triée : a[start..start + len). */
struct run {
    size_t start;
    size_t len;
};

/* Taille de la pile de suites : largement assez grâce aux règles de fusion. */
#define TIM_STACK_MAX 128

/* Étape 1. Mesure la suite naturelle qui commence en a[lo], dans a[lo..hi) : soit
 * croissante au sens large (a[i] <= a[i + 1]), soit strictement décroissante
 * (a[i] > a[i + 1]). Une suite strictement décroissante est retournée sur place pour
 * devenir croissante. Strictement : retourner des valeurs égales casserait la
 * stabilité. Renvoie la longueur de la suite. Précondition : lo < hi. */
size_t count_run(int *a, size_t lo, size_t hi);

/* Étape 2. Renvoie la longueur minimale des suites pour n éléments : n si n < 64 ;
 * sinon, divise n par 2 jusqu'à passer sous 64, puis ajoute 1 si l'un des bits retirés
 * valait 1. Le résultat est alors entre 32 et 64. */
size_t compute_minrun(size_t n);

/* Étape 3. a[lo..mid) et a[mid..hi) sont triés. Les fusionne pour que a[lo..hi) soit
 * trié, de façon stable, en utilisant tmp (au moins mid - lo cases) comme espace de
 * travail. Les autres cases de a ne changent pas. Précondition : lo <= mid <= hi. */
void merge_runs(int *a, int *tmp, size_t lo, size_t mid, size_t hi);

/* Étape 4. Trie a[0..n) par ordre croissant, de façon stable :
 *   - découpe le tableau en suites avec count_run ; une suite plus courte que minrun
 *     est prolongée (jusqu'à minrun éléments, ou la fin) par un tri par insertion ;
 *   - empile chaque suite sur une pile de struct run, puis, tant que l'une des règles
 *     A > B + C (si au moins 3 suites) et B > C n'est pas respectée pour les suites du
 *     sommet (C au sommet, B dessous, A encore dessous), fusionne B avec la plus
 *     petite de ses voisines A et C ;
 *   - à la fin, fusionne toutes les suites restantes, du sommet vers le bas.
 * Un seul tableau de travail, alloué une fois et rendu avant le retour. Si
 * l'allocation échoue, a n'est pas modifié. a peut valoir NULL si n == 0. */
void timsort(int *a, size_t n);

#endif

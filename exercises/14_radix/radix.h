/* Exercice 14 — Tri par base (radix LSD). NE PAS MODIFIER CE FICHIER. */
#ifndef RADIX_H
#define RADIX_H

#include <stddef.h>

/* Clé de tri d'un int x : x converti en entier non signé de 32 bits, puis son bit de
 * poids fort (le bit de signe) inversé. Les clés se rangent alors dans le même ordre
 * que les int : INT_MIN -> 0x00000000, -1 -> 0x7FFFFFFF, 0 -> 0x80000000,
 * INT_MAX -> 0xFFFFFFFF. */

/* Étape 1. Renvoie l'octet numéro b de la clé de tri de x (b = 0 : octet de poids
 * faible, b = 3 : octet de poids fort), entre 0 et 255. Précondition : b < 4. */
unsigned radix_byte(int x, unsigned b);

/* Étape 2. Passe stable de tri par comptage sur l'octet b : écrit dans out[0..n) les
 * valeurs de in[0..n), rangées par octet b croissant ; à octet égal, elles gardent
 * leur ordre de départ. in n'est pas modifié. */
void radix_pass(const int *in, int *out, size_t n, unsigned b);

/* Étape 3. Trie a[0..n) par ordre croissant avec 4 passes, de l'octet 0 à l'octet 3.
 * Un seul tableau auxiliaire de n int, alloué une fois et rendu avant le retour. Si
 * l'allocation échoue, a n'est pas modifié. a peut valoir NULL si n == 0. */
void radix_sort(int *a, size_t n);

#endif

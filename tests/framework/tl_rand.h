/* TriLab — générateur pseudo-aléatoire maison (xorshift64*). NE PAS MODIFIER CE FICHIER.
 * Même graine = même suite de nombres, sur toutes les machines : les échecs sont reproductibles.
 */
#ifndef TL_RAND_H
#define TL_RAND_H

#include <stddef.h>
#include <stdint.h>

#define TL_RAND_DEFAULT_SEED 0x5452494C4142ULL /* « TRILAB » */

static uint64_t tl_rand_state = TL_RAND_DEFAULT_SEED;

/* Fixe la graine. Une graine nulle est remplacée par la graine par défaut. */
static inline void tl_rand_seed(uint64_t seed)
{
    tl_rand_state = seed ? seed : TL_RAND_DEFAULT_SEED;
}

/* Renvoie 64 bits pseudo-aléatoires. */
static inline uint64_t tl_rand_u64(void)
{
    tl_rand_state ^= tl_rand_state >> 12;
    tl_rand_state ^= tl_rand_state << 25;
    tl_rand_state ^= tl_rand_state >> 27;
    return tl_rand_state * 0x2545F4914F6CDD1DULL;
}

/* Renvoie un entier uniforme dans [lo, hi] (bornes incluses, lo <= hi). */
static inline int tl_rand_range(int lo, int hi)
{
    uint64_t span = (uint64_t)((int64_t)hi - (int64_t)lo) + 1u;
    return (int)((int64_t)lo + (int64_t)(tl_rand_u64() % span));
}

/* Renvoie un double uniforme dans [0, 1). */
static inline double tl_rand_unit(void)
{
    return (double)(tl_rand_u64() >> 11) * (1.0 / 9007199254740992.0);
}

/* Remplit a[0..n) avec des entiers uniformes dans [lo, hi]. */
static inline void tl_fill_random(int *a, size_t n, int lo, int hi)
{
    for (size_t i = 0; i < n; i++) {
        a[i] = tl_rand_range(lo, hi);
    }
}

#endif

/* Exercice 17 — tests. NE PAS MODIFIER CE FICHIER. */
#include "intro.h"
#include "tl_check.h"

/* ---------------- Étape 1 · depth_limit ---------------- */

static void check_depth(size_t n, size_t expected)
{
    size_t got = depth_limit(n);
    if (got == expected) {
        return;
    }
    tl_line("n = %zu", n);
    tl_line("attendu : %zu", expected);
    tl_line("obtenu  : %zu", got);
    if (got == 0) {
        tl_symptom("la fonction renvoie 0.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : combien de fois "
                    "peux-tu diviser n par 2 avant d'arriver à 1 ?");
    } else if (got == expected / 2) {
        tl_symptom("c'est log2(n), pas 2 × log2(n).");
        tl_question("Relis le contrat : par combien multiplies-tu le logarithme ?");
    } else if (got == expected + 2) {
        tl_symptom("le logarithme est arrondi vers le haut.");
        tl_question("Combien de fois divises-tu n par 2 : jusqu'à 1, ou jusqu'à 0 ?");
    } else {
        tl_symptom("ce n'est pas 2 × la partie entière de log2(%zu).", n);
        tl_question("Calcule à la main la plus grande puissance de 2 qui ne dépasse pas n.");
    }
    tl_fail();
}

static void depth_petits(void)
{
    check_depth(0, 0);
    check_depth(1, 0);
}

static void depth_puissances(void)
{
    check_depth(2, 2);
    check_depth(4, 4);
    check_depth(16, 8);
    check_depth((size_t)1 << 20, 40);
}

static void depth_entre_deux(void)
{
    check_depth(3, 2);
    check_depth(17, 8);
    check_depth(1000, 18);
    check_depth(((size_t)1 << 20) - 1, 38);
}

/* ---------------- Étape 2 · introsort_loop ---------------- */

/* Vérifie : permutation, cases hors zone intactes, et a[i] <= a[j] dès que j - i >= 16. */
static void check_loop(const int *in, size_t n, size_t lo, size_t hi, size_t depth,
                       const char *what)
{
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta récursion s'arrête-t-elle quand la profondeur atteint 0 ? Ton tri par "
                "tas travaille-t-il bien sur la zone a[lo..hi), et pas sur a[0..hi - lo) ?");
    tl_on_timeout("Quand depth vaut 0, passes-tu bien au tri par tas ? Diminues-tu depth à "
                  "chaque niveau ?");
    introsort_loop(a, lo, hi, depth);
    tl_check_guards(a, n, "introsort_loop");
    for (size_t i = 0; i < n; i++) {
        if ((i < lo || i >= hi) && a[i] != in[i]) {
            tl_line("%s", what);
            tl_symptom("la case a[%zu] est hors de la zone a[%zu..%zu), et elle a changé.", i,
                       lo, hi);
            tl_question("Ton tri par tas et ta partition travaillent-ils bien sur la zone, "
                        "décalée de lo ?");
            tl_fail();
        }
    }
    tl_perm p = tl_check_permutation(in, a, n);
    if (!p.ok) {
        tl_line("%s", what);
        tl_symptom_permutation(p);
        tl_question("Tes échanges gardent-ils les deux valeurs ?");
        tl_fail();
    }
    if (hi - lo <= INTRO_SMALL) {
        if (!tl_ints_equal(a, in, n)) {
            tl_line("%s", what);
            tl_symptom("une zone de %zu éléments a été modifiée, alors qu'elle doit être "
                       "laissée au tri par insertion final.", hi - lo);
            tl_question("À partir de quelle taille ta boucle s'arrête-t-elle ?");
            tl_fail();
        }
        tl_ints_free(a);
        return;
    }
    /* sufmin[i] = plus petite valeur de a[i..hi). */
    int *sufmin = tl_ints_new(n + 1);
    sufmin[hi] = INT_MAX;
    for (size_t i = hi; i > lo; i--) {
        sufmin[i - 1] = a[i - 1] < sufmin[i] ? a[i - 1] : sufmin[i];
    }
    for (size_t i = lo; i + INTRO_SMALL < hi; i++) {
        if (a[i] > sufmin[i + INTRO_SMALL]) {
            size_t j = i + INTRO_SMALL;
            while (a[j] != sufmin[i + INTRO_SMALL]) {
                j++;
            }
            tl_line("%s", what);
            tl_show("obtenu", a, n, i, NULL);
            if (tl_ints_equal(a, in, n)) {
                tl_symptom("la zone n'a pas changé.");
                tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : ta "
                            "boucle tourne-t-elle quand la zone a plus de INTRO_SMALL "
                            "éléments ?");
            } else if (depth == 0) {
                tl_symptom("avec depth = 0, la zone doit être entièrement triée par le tri "
                           "par tas, mais a[%zu] = %d est avant a[%zu] = %d.", i, a[i], j,
                           a[j]);
                tl_question("Quand depth vaut 0, appelles-tu un tri par tas sur toute la "
                            "zone ?");
            } else {
                tl_symptom("a[%zu] = %d est placé %zu cases avant a[%zu] = %d, plus petit.", i,
                           a[i], j - i, j, a[j]);
                tl_question("Après la partition, traites-tu bien les deux côtés du pivot, "
                            "chacun avec depth - 1 ?");
            }
            tl_fail();
        }
    }
    tl_ints_free(sufmin);
    tl_ints_free(a);
}

static void loop_petite_zone(void)
{
    static const int in[] = { 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    check_loop(in, 16, 0, 16, 8, "zone de 16 éléments, depth = 8");
}

static void check_loop_random(size_t n, size_t lo, size_t hi, size_t depth, uint64_t seed,
                              int vmin, int vmax, const char *what)
{
    int *in = tl_ints_new(n);
    tl_rand_seed(seed);
    tl_fill_random(in, n, vmin, vmax);
    check_loop(in, n, lo, hi, depth, what);
    tl_ints_free(in);
}

static void loop_profondeur_zero(void)
{
    check_loop_random(100, 0, 100, 0, 1717, -1000, 1000,
                      "100 éléments, depth = 0 : tout part au tri par tas");
}

static void loop_profondeur_un(void)
{
    check_loop_random(1000, 0, 1000, 1, 1718, -1000, 1000,
                      "1000 éléments, depth = 1 : une partition, puis tri par tas");
}

static void loop_aleatoire(void)
{
    check_loop_random(1000, 0, 1000, 18, 1719, -1000000, 1000000,
                      "1000 éléments aléatoires, depth = 18");
}

static void loop_zone(void)
{
    check_loop_random(110, 5, 105, 0, 1720, -100, 100,
                      "zone a[5..105) de 110 éléments, depth = 0");
}

static void loop_zone_partition(void)
{
    check_loop_random(500, 50, 450, 16, 1721, -100, 100,
                      "zone a[50..450) de 500 éléments, depth = 16");
}

static void loop_tous_egaux_grand(void)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = 7;
    }
    tl_on_timeout("Avec des valeurs toutes égales, la partition est très déséquilibrée : ta "
                  "profondeur diminue-t-elle à chaque niveau, et passes-tu au tri par tas à "
                  "0 ?");
    check_loop(in, N, 0, N, 34, "200 000 valeurs égales, depth = 34");
    tl_ints_free(in);
}

/* ---------------- Étape 3 · introsort ---------------- */

TL_SORT_SUITE_DEFINE(introsort, 200000)

static void sort_motif(int kind)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = kind == 0 ? 7 : kind == 1 ? (int)i : (int)(i % 3);
    }
    tl_on_timeout("introsort appelle-t-il introsort_loop avec depth_limit(n), puis un seul "
                  "tri par insertion sur tout le tableau ?");
    tl_check_sort_int("introsort", introsort, in, N, NULL);
    tl_ints_free(in);
}

static void sort_tous_egaux_grand(void)
{
    sort_motif(0);
}

static void sort_deja_trie_grand(void)
{
    sort_motif(1);
}

static void sort_trois_valeurs_grand(void)
{
    sort_motif(2);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "depth_limit", false },
    { 2, "introsort_loop", false },
    { 3, "introsort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, depth_petits, "n = 0 et n = 1"),
    TL_TEST(1, depth_puissances, "puissances de 2"),
    TL_TEST(1, depth_entre_deux, "entre deux puissances de 2"),

    TL_TEST(2, loop_petite_zone, "zone de INTRO_SMALL éléments : rien à faire"),
    TL_TEST(2, loop_profondeur_zero, "depth = 0 : tri par tas"),
    TL_TEST(2, loop_profondeur_un, "depth = 1"),
    TL_TEST(2, loop_aleatoire, "1000 éléments aléatoires"),
    TL_TEST(2, loop_zone, "zone a[5..105), depth = 0"),
    TL_TEST(2, loop_zone_partition, "zone a[50..450), depth = 16"),
    TL_TEST(2, loop_tous_egaux_grand, "200 000 valeurs égales"),

    TL_SORT_SUITE_ENTRIES(3, introsort),
    TL_TEST(3, sort_tous_egaux_grand, "200 000 valeurs égales"),
    TL_TEST(3, sort_deja_trie_grand, "200 000 éléments déjà triés"),
    TL_TEST(3, sort_trois_valeurs_grand, "200 000 valeurs parmi 3"),
};

TL_MAIN("17", "Introsort", STEPS, TESTS)

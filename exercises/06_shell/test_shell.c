/* Exercice 06 — tests. NE PAS MODIFIER CE FICHIER. */
#include "shell.h"
#include "tl_check.h"

/* ---------------- Étape 1 · gapped_insertion ---------------- */

/* Affiche une sous-suite a[r], a[r + gap], ... */
static void show_subsequence(const char *label, const int *a, size_t n, size_t r, size_t gap)
{
    printf("    %s{", label);
    for (size_t i = r; i < n; i += gap) {
        printf(i > r ? ", %d" : "%d", a[i]);
    }
    printf("}\n");
}

static void check_gapped(const int *in, size_t n, size_t gap, const int *expected)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta boucle tourne-t-elle quand même ?");
        gapped_insertion(NULL, 0, gap);
        return;
    }
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Que devient ton indice quand il recule de gap alors qu'il est déjà plus "
                "petit que gap ?");
    tl_on_timeout("Que devient un size_t auquel on retire gap alors qu'il vaut moins que "
                  "gap ?");
    gapped_insertion(a, n, gap);
    tl_check_guards(a, n, "gapped_insertion");
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }
    tl_line("écart   : %zu", gap);
    tl_report_ints(in, expected, a, n, NULL, NULL);
    if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : ta boucle "
                    "tourne-t-elle au moins une fois ?");
        tl_fail();
    }
    tl_perm p = tl_check_permutation(in, a, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Où est gardée la valeur à insérer pendant que les autres se "
                    "décalent ?");
        tl_fail();
    }
    if (gap > 1 && tl_first_unsorted(a, n) == n) {
        tl_symptom("le tableau est entièrement trié : ta fonction a trié tout le tableau, "
                   "pas chaque sous-suite d'écart %zu.", gap);
        tl_question("Quelles cases ta fonction compare-t-elle : des voisines, ou des cases "
                    "distantes de gap ?");
        tl_fail();
    }
    for (size_t r = 0; r < gap && r < n; r++) {
        for (size_t i = r; i < n; i += gap) {
            if (a[i] != expected[i]) {
                tl_symptom("la sous-suite qui commence en a[%zu] n'est pas celle attendue.",
                           r);
                show_subsequence("avant   : ", in, n, r, gap);
                show_subsequence("attendu : ", expected, n, r, gap);
                show_subsequence("obtenu  : ", a, n, r, gap);
                tl_question("Déroule l'insertion de chaque valeur de cette sous-suite : "
                            "jusqu'où recule-t-elle, et par pas de combien ?");
                tl_fail();
            }
        }
    }
    tl_fail();
}

static void gi_vide(void)
{
    check_gapped(NULL, 0, 4, NULL);
}

static void gi_ecart_1(void)
{
    static const int in[] = { 3, 1, 2 };
    static const int expected[] = { 1, 2, 3 };
    check_gapped(in, 3, 1, expected);
}

static void gi_ecart_1_inverse(void)
{
    static const int in[] = { 5, 4, 3, 2, 1, 0 };
    static const int expected[] = { 0, 1, 2, 3, 4, 5 };
    check_gapped(in, 6, 1, expected);
}

static void gi_ecart_2(void)
{
    static const int in[] = { 4, 9, 1, 7, 3, 2, 8 };
    static const int expected[] = { 1, 2, 3, 7, 4, 9, 8 };
    check_gapped(in, 7, 2, expected);
}

static void gi_ecart_3(void)
{
    static const int in[] = { 6, 1, 9, 4, 8, 2, 0, 7 };
    static const int expected[] = { 0, 1, 2, 4, 7, 9, 6, 8 };
    check_gapped(in, 8, 3, expected);
}

static void gi_egaux(void)
{
    static const int in[] = { 3, 1, 2, 1, 1, 2 };
    static const int expected[] = { 1, 1, 2, 1, 3, 2 };
    check_gapped(in, 6, 2, expected);
}

static void gi_extremes(void)
{
    static const int in[] = { INT_MAX, 0, INT_MIN, -1, 5 };
    static const int expected[] = { INT_MIN, -1, 5, 0, INT_MAX };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_gapped(in, 5, 2, expected);
}

static void gi_ecart_trop_grand(void)
{
    static const int in[] = { 3, 2, 1 };
    check_gapped(in, 3, 5, in);
}

/* ---------------- Étape 2 · shell_sort ---------------- */

TL_SORT_SUITE_DEFINE(shell_sort, 100000)

/* Ordre inverse : sans les grands écarts, ce serait un tri par insertion, bien trop lent. */
static void sort_inverse_grand(void)
{
    enum { N = 300000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = (int)(N - i);
    }
    tl_on_timeout("Utilises-tu les grands écarts de la suite avant l'écart 1 ? Lesquels "
                  "sont plus petits que n ?");
    tl_check_sort_int("shell_sort", shell_sort, in, N, NULL);
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "gapped_insertion", false },
    { 2, "shell_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, gi_vide, "n = 0 avec a = NULL"),
    TL_TEST(1, gi_ecart_1, "écart 1 sur {3, 1, 2}"),
    TL_TEST(1, gi_ecart_1_inverse, "écart 1 sur {5, 4, 3, 2, 1, 0}"),
    TL_TEST(1, gi_ecart_2, "écart 2 sur {4, 9, 1, 7, 3, 2, 8}"),
    TL_TEST(1, gi_ecart_3, "écart 3 sur {6, 1, 9, 4, 8, 2, 0, 7}"),
    TL_TEST(1, gi_egaux, "écart 2 sur {3, 1, 2, 1, 1, 2}"),
    TL_TEST(1, gi_extremes, "écart 2 sur {INT_MAX, 0, INT_MIN, -1, 5}"),
    TL_TEST(1, gi_ecart_trop_grand, "écart 5 sur 3 éléments"),

    TL_SORT_SUITE_ENTRIES(2, shell_sort),
    TL_TEST(2, sort_inverse_grand, "300 000 éléments en ordre inverse"),
};

TL_MAIN("06", "Tri de Shell", STEPS, TESTS)

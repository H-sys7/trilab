/* Exercice 05 — tests. NE PAS MODIFIER CE FICHIER. */
#include "comb.h"
#include "tl_check.h"

/* ---------------- Étape 1 · comb_next_gap ---------------- */

static void check_gap(size_t gap, size_t expected)
{
    size_t got = comb_next_gap(gap);
    if (got == expected) {
        return;
    }
    tl_line("entrée  : gap = %zu", gap);
    tl_line("attendu : %zu", expected);
    tl_line("obtenu  : %zu", got);
    if (got == 0 && expected > 1) {
        tl_symptom("la fonction renvoie 0.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : écris ton "
                    "calcul étape par étape pour gap = %zu. Quel est le type de chaque "
                    "résultat intermédiaire ?", gap);
    } else if (got == 0) {
        tl_symptom("l'écart est descendu sous 1.");
        tl_question("Que renvoie ta fonction quand la division donne moins de 1 ?");
    } else if (got == expected + 1) {
        tl_symptom("le résultat est arrondi vers le haut, au lieu de vers le bas.");
        tl_question("Comment ta conversion en entier traite-t-elle les chiffres après la "
                    "virgule ?");
    } else if (got == gap && gap > 1) {
        tl_symptom("l'écart n'a pas diminué.");
        tl_question("Écris ton calcul étape par étape pour gap = %zu : quel est le type de "
                    "chaque résultat intermédiaire ?", gap);
    } else {
        tl_symptom("%zu n'est pas %zu / 1,3 arrondi vers le bas.", got, gap);
        tl_question("Écris ton calcul étape par étape pour gap = %zu : quelle valeur "
                    "obtiens-tu à chaque étape, et de quel type est-elle ?", gap);
    }
    tl_fail();
}

static void gap_dix(void)
{
    check_gap(10, 7);
}

static void gap_sept(void)
{
    check_gap(7, 5);
}

static void gap_quatre(void)
{
    check_gap(4, 3);
}

static void gap_trois(void)
{
    check_gap(3, 2);
}

static void gap_deux(void)
{
    check_gap(2, 1);
}

static void gap_un(void)
{
    check_gap(1, 1);
}

static void gap_cent(void)
{
    check_gap(100, 76);
}

static void gap_million(void)
{
    check_gap(1000000, 769230);
}

/* ---------------- Étape 2 · comb_pass ---------------- */

/* Vérifie une passe d'écart gap ; known (peut valoir NULL) : résultat d'une erreur connue. */
static void check_pass(const int *in, size_t n, size_t gap, const int *expected,
                       size_t expected_ret, const int *known, const char *known_symptom,
                       const char *known_question)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta boucle tourne-t-elle quand même ? Que vaut n - gap "
                    "quand gap est plus grand que n, avec des size_t ?");
        size_t ret = comb_pass(NULL, 0, gap);
        if (ret != 0) {
            tl_line("entrée  : a = NULL, n = 0, gap = %zu", gap);
            tl_line("attendu : 0");
            tl_line("obtenu  : %zu", ret);
            tl_symptom("un tableau vide ne demande aucun échange.");
            tl_question("Que compte ta fonction quand il n'y a aucune paire ?");
            tl_fail();
        }
        return;
    }
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle lit-elle a[i + gap] au-delà de la fin ? Si gap est plus grand "
                "que n, que vaut n - gap avec des size_t ?");
    tl_on_timeout("Si gap est plus grand que n, que vaut n - gap avec des size_t ?");
    size_t ret = comb_pass(a, n, gap);
    tl_check_guards(a, n, "comb_pass");
    if (ret == expected_ret && tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }

    char note_exp[64];
    char note_got[64];
    snprintf(note_exp, sizeof note_exp, "(échanges : %zu)", expected_ret);
    snprintf(note_got, sizeof note_got, "(échanges : %zu)", ret);
    tl_line("écart   : %zu", gap);
    tl_report_ints(in, expected, a, n, note_exp, note_got);

    if (known != NULL && tl_ints_equal(a, known, n)) {
        tl_symptom("%s", known_symptom);
        tl_question("%s", known_question);
        tl_fail();
    }
    if (ret == 0 && tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé et la fonction renvoie 0.");
        tl_question("Si tu n'as pas encore commencé l'étape 2, c'est normal. Sinon : "
                    "combien de paires (a[i], a[i + %zu]) existe-t-il pour %zu éléments ?",
                    gap, n);
        tl_fail();
    }
    tl_perm p = tl_check_permutation(in, a, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Que contient *x juste après la première affectation de ton swap ?");
        tl_fail();
    }
    if (tl_ints_equal(a, expected, n)) {
        tl_symptom("le tableau est correct, mais la fonction renvoie %zu au lieu de %zu.",
                   ret, expected_ret);
        tl_question("À quel moment ton compteur augmente-t-il : à chaque comparaison, "
                    "ou seulement quand un échange a lieu ?");
        tl_fail();
    }
    tl_symptom_diff(in, expected, a, n);
    tl_question("Écris la liste des paires (i, i + %zu) que ta boucle compare. Sont-ce "
                "bien des cases à distance %zu ?", gap, gap);
    tl_fail();
}

static void pass_vide(void)
{
    check_pass(NULL, 0, 1, NULL, 0, NULL, NULL, NULL);
}

static void pass_ecart_1(void)
{
    static const int in[] = { 3, 1, 2, 9, 4 };
    static const int expected[] = { 1, 2, 3, 4, 9 };
    check_pass(in, 5, 1, expected, 3, NULL, NULL, NULL);
}

static void pass_ecart_2(void)
{
    static const int in[] = { 5, 1, 4, 2, 3 };
    static const int expected[] = { 4, 1, 3, 2, 5 };
    check_pass(in, 5, 2, expected, 2, NULL, NULL, NULL);
}

static void pass_ecart_3(void)
{
    static const int in[] = { 9, 8, 7, 6, 5, 4 };
    static const int expected[] = { 6, 5, 4, 9, 8, 7 };
    check_pass(in, 6, 3, expected, 3, NULL, NULL, NULL);
}

static void pass_derniere_paire(void)
{
    static const int in[] = { 1, 2, 3, 9, 4, 5, 0 };
    static const int expected[] = { 1, 2, 3, 0, 4, 5, 9 };
    check_pass(in, 7, 3, expected, 1, in, "la dernière paire (a[3], a[6]) = (9, 0) n'a pas "
               "été traitée.", "Pour 7 éléments et un écart de 3, quelle est la dernière "
               "paire de cases à comparer ? Ta boucle l'atteint-elle ?");
}

static void pass_ecart_egal_n(void)
{
    static const int in[] = { 3, 2, 1 };
    check_pass(in, 3, 3, in, 0, NULL, NULL, NULL);
}

static void pass_ecart_trop_grand(void)
{
    static const int in[] = { 3, 2, 1 };
    check_pass(in, 3, 5, in, 0, NULL, NULL, NULL);
}

static void pass_egaux(void)
{
    static const int in[] = { 2, 5, 2, 1 };
    static const int expected[] = { 2, 1, 2, 5 };
    check_pass(in, 4, 2, expected, 1, NULL, NULL, NULL);
}

static void pass_extremes(void)
{
    static const int in[] = { INT_MAX, 0, INT_MIN };
    static const int expected[] = { INT_MIN, 0, INT_MAX };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_pass(in, 3, 2, expected, 1, NULL, NULL, NULL);
}

/* ---------------- Étape 3 · comb_sort ---------------- */

TL_SORT_SUITE_DEFINE(comb_sort, 100000)

/* Ordre inverse : les grands écarts déplacent les valeurs loin d'un coup. */
static void sort_inverse_grand(void)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = (int)(N - i);
    }
    tl_on_timeout("Ton écart part-il bien de n, et diminue-t-il à chaque passe ? Tant "
                  "qu'il vaut plus que 1, combien de passes fais-tu avec le même écart ?");
    tl_check_sort_int("comb_sort", comb_sort, in, N, NULL);
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "comb_next_gap", false },
    { 2, "comb_pass", false },
    { 3, "comb_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, gap_dix, "écart suivant de 10"),
    TL_TEST(1, gap_sept, "écart suivant de 7"),
    TL_TEST(1, gap_quatre, "écart suivant de 4"),
    TL_TEST(1, gap_trois, "écart suivant de 3"),
    TL_TEST(1, gap_deux, "écart suivant de 2"),
    TL_TEST(1, gap_un, "écart suivant de 1"),
    TL_TEST(1, gap_cent, "écart suivant de 100"),
    TL_TEST(1, gap_million, "écart suivant de 1 000 000"),

    TL_TEST(2, pass_vide, "n = 0 avec a = NULL"),
    TL_TEST(2, pass_ecart_1, "écart 1 sur {3, 1, 2, 9, 4}"),
    TL_TEST(2, pass_ecart_2, "écart 2 sur {5, 1, 4, 2, 3}"),
    TL_TEST(2, pass_ecart_3, "écart 3 sur {9, 8, 7, 6, 5, 4}"),
    TL_TEST(2, pass_derniere_paire, "écart 3 sur {1, 2, 3, 9, 4, 5, 0}"),
    TL_TEST(2, pass_ecart_egal_n, "écart 3 sur 3 éléments"),
    TL_TEST(2, pass_ecart_trop_grand, "écart 5 sur 3 éléments"),
    TL_TEST(2, pass_egaux, "écart 2 sur {2, 5, 2, 1}"),
    TL_TEST(2, pass_extremes, "écart 2 sur {INT_MAX, 0, INT_MIN}"),

    TL_SORT_SUITE_ENTRIES(3, comb_sort),
    TL_TEST(3, sort_inverse_grand, "200 000 éléments en ordre inverse"),
};

TL_MAIN("05", "Tri à peigne", STEPS, TESTS)

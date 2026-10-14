/* Exercice 08 — tests. NE PAS MODIFIER CE FICHIER. */
#include "merge.h"
#include "tl_check.h"

#define TMP_MARK 77777 /* valeur de départ des cases de tmp */

static int *tmp_new(size_t n)
{
    int *tmp = tl_ints_new(n);
    for (size_t i = 0; i < n; i++) {
        tmp[i] = TMP_MARK;
    }
    return tmp;
}

/* Vérifie qu'aucune case de a hors de [lo..hi), ni de tmp hors de [lo..hi), n'a changé. */
static void check_outside(const int *in, const int *a, const int *tmp, size_t n, size_t lo,
                          size_t hi)
{
    for (size_t i = 0; i < n; i++) {
        if (i >= lo && i < hi) {
            continue;
        }
        if (a[i] != in[i]) {
            tl_symptom("la case a[%zu] est hors de la zone a[%zu..%zu), et elle a changé.", i,
                       lo, hi);
            tl_question("Quelles cases ta fonction a-t-elle le droit de toucher ?");
            tl_fail();
        }
        if (tmp[i] != TMP_MARK) {
            tl_symptom("la case tmp[%zu] est hors de tmp[%zu..%zu), et elle a été utilisée.",
                       i, lo, hi);
            tl_question("Quelles cases de tmp ta fonction a-t-elle le droit d'utiliser ?");
            tl_fail();
        }
    }
}

/* ---------------- Étape 1 · merge ---------------- */

static void check_merge(const int *in, size_t n, size_t lo, size_t mid, size_t hi,
                        const int *expected)
{
    int *a = tl_ints_copy(in, n);
    int *tmp = tmp_new(n);
    tl_on_crash("Quand une moitié est épuisée, ta fonction lit-elle encore une case de "
                "cette moitié ?");
    merge(a, tmp, lo, mid, hi);
    tl_check_guards(a, n, "merge");
    tl_check_guards(tmp, n, "merge (dans tmp)");
    if (tl_ints_equal(a, expected, n)) {
        check_outside(in, a, tmp, n, lo, hi);
        tl_ints_free(a);
        tl_ints_free(tmp);
        return;
    }
    tl_line("zones   : a[%zu..%zu) et a[%zu..%zu)", lo, mid, mid, hi);
    tl_report_ints(in, expected, a, n, NULL, NULL);
    if (tl_ints_equal(a, in, n)) {
        if (hi > lo && tl_ints_equal(tmp + lo, expected + lo, hi - lo)) {
            tl_show("tmp", tmp, n, lo, NULL);
            tl_symptom("tmp contient le bon résultat, mais a n'a pas changé.");
            tl_question("Une fois la fusion faite dans tmp, où le résultat doit-il se "
                        "trouver ?");
        } else {
            tl_symptom("le tableau n'a pas changé.");
            tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : à chaque "
                        "tour, quelles têtes de moitié compares-tu ?");
        }
        tl_fail();
    }
    check_outside(in, a, tmp, n, lo, hi);
    tl_perm p = tl_check_permutation(in, a, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        if (p.has_extra && p.extra == TMP_MARK) {
            tl_line("(%d est la valeur de départ des cases de tmp.)", TMP_MARK);
        }
        tl_question("Quand une des deux moitiés est épuisée, que deviennent les éléments "
                    "qui restent dans l'autre ?");
        tl_fail();
    }
    tl_symptom_diff(in, expected, a, n);
    tl_question("Déroule ta fusion à la main : à chaque tour, quelles têtes compares-tu, et "
                "laquelle prends-tu ?");
    tl_fail();
}

static void merge_exemple(void)
{
    static const int in[] = { 1, 4, 7, 2, 3, 9 };
    static const int expected[] = { 1, 2, 3, 4, 7, 9 };
    check_merge(in, 6, 0, 3, 6, expected);
}

static void merge_gauche_epuisee(void)
{
    static const int in[] = { 1, 2, 3, 4, 5, 6 };
    check_merge(in, 6, 0, 3, 6, in);
}

static void merge_droite_epuisee(void)
{
    static const int in[] = { 4, 5, 6, 1, 2, 3 };
    static const int expected[] = { 1, 2, 3, 4, 5, 6 };
    check_merge(in, 6, 0, 3, 6, expected);
}

static void merge_gauche_vide(void)
{
    static const int in[] = { 2, 5 };
    check_merge(in, 2, 0, 0, 2, in);
}

static void merge_droite_vide(void)
{
    static const int in[] = { 2, 5 };
    check_merge(in, 2, 0, 2, 2, in);
}

static void merge_tailles_differentes(void)
{
    static const int in[] = { 8, 1, 2, 3, 5, 9 };
    static const int expected[] = { 1, 2, 3, 5, 8, 9 };
    check_merge(in, 6, 0, 1, 6, expected);
}

static void merge_zone(void)
{
    static const int in[] = { 9, 3, 8, 1, 5, 0 };
    static const int expected[] = { 9, 1, 3, 5, 8, 0 };
    check_merge(in, 6, 1, 3, 5, expected);
}

static void merge_doublons(void)
{
    static const int in[] = { 1, 3, 3, 3, 4 };
    static const int expected[] = { 1, 3, 3, 3, 4 };
    check_merge(in, 5, 0, 3, 5, expected);
}

static void merge_extremes(void)
{
    static const int in[] = { INT_MIN, INT_MAX, INT_MIN, 0 };
    static const int expected[] = { INT_MIN, INT_MIN, 0, INT_MAX };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_merge(in, 4, 0, 2, 4, expected);
}

/* ---------------- Étape 2 · merge_sort_rec ---------------- */

static void check_rec(const int *in, size_t n, size_t lo, size_t hi, const int *expected)
{
    int *a = tl_ints_copy(in, n);
    int *tmp = tmp_new(n);
    tl_on_crash("Quel est le cas de base de ta récursion ? Une zone d'un seul élément "
                "s'arrête-t-elle ?");
    merge_sort_rec(a, tmp, lo, hi);
    tl_check_guards(a, n, "merge_sort_rec");
    tl_check_guards(tmp, n, "merge_sort_rec (dans tmp)");
    if (tl_ints_equal(a, expected, n)) {
        check_outside(in, a, tmp, n, lo, hi);
        tl_ints_free(a);
        tl_ints_free(tmp);
        return;
    }
    tl_line("zone    : a[%zu..%zu)", lo, hi);
    tl_report_ints(in, expected, a, n, NULL, NULL);
    if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : ta fonction "
                    "appelle-t-elle merge après les deux appels récursifs ?");
        tl_fail();
    }
    check_outside(in, a, tmp, n, lo, hi);
    tl_perm p = tl_check_permutation(in, a, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Tes tests de l'étape 1 passent-ils tous ? Sinon, commence par merge.");
        tl_fail();
    }
    tl_symptom_diff(in, expected, a, n);
    tl_question("Les deux moitiés que tu tries puis fusionnes couvrent-elles exactement "
                "a[lo..hi), sans trou ni chevauchement ?");
    tl_fail();
}

static void rec_vide(void)
{
    static const int in[] = { 7 };
    check_rec(in, 1, 0, 0, in);
}

static void rec_un(void)
{
    static const int in[] = { 5 };
    check_rec(in, 1, 0, 1, in);
}

static void rec_deux(void)
{
    static const int in[] = { 2, 1 };
    static const int expected[] = { 1, 2 };
    check_rec(in, 2, 0, 2, expected);
}

static void rec_impair(void)
{
    static const int in[] = { 5, 2, 9, 1, 7 };
    static const int expected[] = { 1, 2, 5, 7, 9 };
    check_rec(in, 5, 0, 5, expected);
}

static void rec_zone(void)
{
    static const int in[] = { 100, 4, 3, 2, 1, -100 };
    static const int expected[] = { 100, 1, 2, 3, 4, -100 };
    check_rec(in, 6, 1, 5, expected);
}

/* Tout le tableau, avec un tmp alloué par le test. */
static void rec_whole(int *a, size_t n)
{
    int *tmp = tl_malloc(n * sizeof(int));
    merge_sort_rec(a, tmp, 0, n);
    tl_free(tmp);
}

static void rec_aleatoire(void)
{
    tl_on_crash("Quel est le cas de base de ta récursion ? Une zone d'un seul élément "
                "s'arrête-t-elle ?");
    tl_check_sort_random("merge_sort_rec", rec_whole, 1000, 8008, -1000, 1000);
}

static void rec_grand(void)
{
    tl_on_timeout("Tes deux appels récursifs portent-ils sur des moitiés de la zone, ou "
                  "sur des zones presque aussi grandes qu'elle ?");
    tl_check_sort_random("merge_sort_rec", rec_whole, 200000, 8009, INT_MIN, INT_MAX);
}

/* ---------------- Étape 3 · merge_sort ---------------- */

TL_SORT_SUITE_DEFINE(merge_sort, 200000)

static void sort_cinq_cent_mille(void)
{
    tl_on_timeout("Ta fonction alloue-t-elle une seule fois, ou à chaque fusion ?");
    tl_check_sort_random("merge_sort", merge_sort, 500000, 8010, INT_MIN, INT_MAX);
}

/* ---------------- Bonus · merge_sort_bottom_up ---------------- */

TL_SORT_SUITE_DEFINE(merge_sort_bottom_up, 200000)

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "merge", false },
    { 2, "merge_sort_rec", false },
    { 3, "merge_sort", false },
    { 4, "merge_sort_bottom_up", true },
};

static const tl_test TESTS[] = {
    TL_TEST(1, merge_exemple, "fusion de {1, 4, 7} et {2, 3, 9}"),
    TL_TEST(1, merge_gauche_epuisee, "la moitié gauche s'épuise d'abord"),
    TL_TEST(1, merge_droite_epuisee, "la moitié droite s'épuise d'abord"),
    TL_TEST(1, merge_gauche_vide, "moitié gauche vide"),
    TL_TEST(1, merge_droite_vide, "moitié droite vide"),
    TL_TEST(1, merge_tailles_differentes, "moitiés de tailles 1 et 5"),
    TL_TEST(1, merge_zone, "fusion dans la zone a[1..5)"),
    TL_TEST(1, merge_doublons, "doublons"),
    TL_TEST(1, merge_extremes, "INT_MIN et INT_MAX"),

    TL_TEST(2, rec_vide, "zone vide"),
    TL_TEST(2, rec_un, "zone d'un élément"),
    TL_TEST(2, rec_deux, "{2, 1}"),
    TL_TEST(2, rec_impair, "{5, 2, 9, 1, 7}"),
    TL_TEST(2, rec_zone, "zone a[1..5) de {100, 4, 3, 2, 1, -100}"),
    TL_TEST(2, rec_aleatoire, "1000 éléments aléatoires"),
    TL_TEST(2, rec_grand, "200 000 éléments aléatoires"),

    TL_SORT_SUITE_ENTRIES(3, merge_sort),
    TL_TEST(3, sort_cinq_cent_mille, "500 000 éléments aléatoires"),

    TL_SORT_SUITE_ENTRIES(4, merge_sort_bottom_up),
};

TL_MAIN("08", "Tri fusion", STEPS, TESTS)

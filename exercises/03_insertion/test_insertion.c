/* Exercice 03 — tests. NE PAS MODIFIER CE FICHIER. */
#include "insertion.h"
#include "tl_check.h"

/* ---------------- Étape 1 · insert_into_sorted ---------------- */

/* in contient len + 1 valeurs : a[0..len) trié, puis la valeur à insérer. */
static void check_insert(const int *in, size_t len, const int *expected)
{
    size_t n = len + 1;
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Que vaut ton indice quand l'élément à insérer est plus petit que tous "
                "les autres ?");
    insert_into_sorted(a, len);
    tl_check_guards(a, n, "insert_into_sorted");
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }
    tl_line("insérer a[%zu] = %d dans a[0..%zu)", len, in[len], len);
    tl_report_ints(in, expected, a, n, NULL, NULL);
    if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé : %d est resté dans la dernière case.", in[len]);
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
    size_t i = tl_first_unsorted(a, n);
    if (i < n) {
        tl_symptom("toutes les valeurs sont là, mais a[%zu] = %d est placé avant a[%zu] = "
                   "%d.", i, a[i], i + 1, a[i + 1]);
    } else {
        tl_symptom_diff(in, expected, a, n);
    }
    tl_question("Déroule ta fonction à la main sur cette entrée : à quel moment ta boucle "
                "s'arrête-t-elle, et pourquoi ?");
    tl_fail();
}

static void ins_vide(void)
{
    static const int in[] = { 42 };
    check_insert(in, 0, in);
}

static void ins_deja_place(void)
{
    static const int in[] = { 1, 3, 5, 7 };
    check_insert(in, 3, in);
}

static void ins_deux(void)
{
    static const int in[] = { 5, 1 };
    static const int expected[] = { 1, 5 };
    check_insert(in, 1, expected);
}

static void ins_milieu(void)
{
    static const int in[] = { 1, 3, 5, 7, 4 };
    static const int expected[] = { 1, 3, 4, 5, 7 };
    check_insert(in, 4, expected);
}

static void ins_debut(void)
{
    static const int in[] = { 2, 5, 7, 0 };
    static const int expected[] = { 0, 2, 5, 7 };
    check_insert(in, 3, expected);
}

static void ins_egal(void)
{
    static const int in[] = { 1, 2, 2, 3, 2 };
    static const int expected[] = { 1, 2, 2, 2, 3 };
    check_insert(in, 4, expected);
}

static void ins_extremes(void)
{
    static const int in[] = { INT_MIN, -1, INT_MAX, INT_MIN };
    static const int expected[] = { INT_MIN, INT_MIN, -1, INT_MAX };
    check_insert(in, 3, expected);
}

static void ins_long(void)
{
    static const int in[] = { -8, -3, 0, 2, 4, 4, 9, 11, 15, 20, 27, 31, 40, 52, 60, 5 };
    static const int expected[] = { -8, -3, 0, 2, 4, 4, 5, 9, 11, 15, 20, 27, 31, 40, 52,
                                    60 };
    check_insert(in, 15, expected);
}

/* ---------------- Étape 2 · insertion_sort ---------------- */

TL_SORT_SUITE_DEFINE(insertion_sort, 5000)

/* Presque trié : chaque insertion doit s'arrêter vite, sinon le tri dépasse le temps. */
static void sort_presque_trie(void)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = (int)i;
    }
    for (size_t i = 0; i + 1 < N; i += 1000) {
        int t = in[i];
        in[i] = in[i + 1];
        in[i + 1] = t;
    }
    tl_on_timeout("Quand la valeur à insérer est déjà plus grande que sa voisine de "
                  "gauche, ta boucle s'arrête-t-elle tout de suite ?");
    tl_check_sort_int("insertion_sort", insertion_sort, in, N, NULL);
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "insert_into_sorted", false },
    { 2, "insertion_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, ins_vide, "insérer 42 dans une partie vide"),
    TL_TEST(1, ins_deja_place, "insérer 7 dans {1, 3, 5}"),
    TL_TEST(1, ins_deux, "insérer 1 dans {5}"),
    TL_TEST(1, ins_milieu, "insérer 4 dans {1, 3, 5, 7}"),
    TL_TEST(1, ins_debut, "insérer 0 dans {2, 5, 7}"),
    TL_TEST(1, ins_egal, "insérer 2 dans {1, 2, 2, 3}"),
    TL_TEST(1, ins_extremes, "insérer INT_MIN dans {INT_MIN, -1, INT_MAX}"),
    TL_TEST(1, ins_long, "insérer 5 dans 15 valeurs"),

    TL_SORT_SUITE_ENTRIES(2, insertion_sort),
    TL_TEST_T(2, sort_presque_trie, "200 000 éléments presque triés", 3),
};

TL_MAIN("03", "Tri par insertion", STEPS, TESTS)

/* Exercice 02 — tests. NE PAS MODIFIER CE FICHIER. */
#include "selection.h"
#include "tl_check.h"

/* ---------------- Étape 1 · min_index_from ---------------- */

static void check_min_from(const int *in, size_t n, size_t from, size_t expected)
{
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle lit-elle une case en dehors de a[0..n) ?");
    size_t got = min_index_from(a, from, n);
    tl_check_guards(a, n, "min_index_from");
    if (!tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau a été modifié, alors que min_index_from doit seulement le "
                   "lire.");
        tl_question("Quelle instruction de ta fonction écrit dans le tableau ?");
        tl_fail();
    }
    if (got == expected) {
        tl_ints_free(a);
        return;
    }
    tl_show("entrée", in, n, got < n ? got : expected, NULL);
    tl_line("from    : %zu", from);
    tl_line("attendu : indice %zu (valeur %d)", expected, in[expected]);
    if (got >= n) {
        tl_line("obtenu  : indice %zu", got);
        tl_symptom("l'indice renvoyé est en dehors du tableau.");
        tl_question("Pour %zu éléments, quels sont les indices valides ?", n);
    } else if (got < from) {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("l'indice renvoyé est avant from = %zu : cette case ne fait pas partie "
                   "de la zone de recherche.", from);
        tl_question("Ta recherche commence-t-elle bien à from ? Si tu n'as pas commencé "
                    "l'étape 1, c'est normal.");
    } else if (in[got] == in[expected]) {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("bonne valeur, mais ce n'est pas sa première apparition.");
        tl_question("Quand tu croises une valeur égale au plus petit trouvé jusqu'ici, "
                    "lequel des deux indices gardes-tu ?");
    } else if (expected == n - 1) {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("le minimum est dans la dernière case, et il n'a pas été retenu.");
        tl_question("Ta boucle examine-t-elle la toute dernière case ?");
    } else if (got == 0 && from == 0) {
        tl_line("obtenu  : indice 0 (valeur %d)", in[0]);
        tl_symptom("la fonction renvoie 0, la valeur de départ.");
        tl_question("Ta fonction parcourt-elle le tableau ? Si tu n'as pas commencé "
                    "l'étape 1, c'est normal.");
    } else {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("%d n'est pas la plus petite valeur de a[%zu..%zu) : %d est plus petit.",
                   in[got], from, n, in[expected]);
        tl_question("Déroule ta fonction à la main sur cette entrée : quel indice "
                    "retiens-tu après chaque case ?");
    }
    tl_fail();
}

static void min_depuis_debut(void)
{
    static const int in[] = { 3, 1, 2 };
    check_min_from(in, TL_LEN(in), 0, 1);
}

static void min_depuis_milieu(void)
{
    static const int in[] = { 0, 5, 3, 4 };
    check_min_from(in, TL_LEN(in), 1, 2);
}

static void min_ignore_avant(void)
{
    static const int in[] = { 1, 5, 4, 6 };
    check_min_from(in, TL_LEN(in), 1, 2);
}

static void min_une_case(void)
{
    static const int in[] = { 9, 8, 7 };
    check_min_from(in, TL_LEN(in), 2, 2);
}

static void min_a_la_fin(void)
{
    static const int in[] = { 5, 4, 3, 2, 1 };
    check_min_from(in, TL_LEN(in), 1, 4);
}

static void min_egalites(void)
{
    static const int in[] = { 4, 2, 3, 2 };
    check_min_from(in, TL_LEN(in), 1, 1);
}

static void min_extremes(void)
{
    static const int in[] = { INT_MIN, 0, INT_MAX, INT_MIN };
    check_min_from(in, TL_LEN(in), 1, 3);
}

static void min_aleatoire(void)
{
    enum { N = 1000, FROM = 300, POS = 700 };
    int *in = tl_ints_new(N);
    tl_rand_seed(5005);
    tl_fill_random(in, N, 0, 1000000);
    in[100] = -2;
    in[POS] = -1;
    check_min_from(in, N, FROM, POS);
    tl_ints_free(in);
}

/* ---------------- Étape 2 · selection_sort ---------------- */

TL_SORT_SUITE_DEFINE(selection_sort, 5000)

/* Le minimum est en dernière position : il doit arriver en tête dès le premier tour. */
static void sort_min_au_bout(void)
{
    static const int in[] = { 2, 3, 4, 5, 6, 7, 1 };
    tl_check_sort_int("selection_sort", selection_sort, in, TL_LEN(in), NULL);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "min_index_from", false },
    { 2, "selection_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, min_depuis_debut, "recherche depuis from = 0"),
    TL_TEST(1, min_depuis_milieu, "recherche depuis from = 1"),
    TL_TEST(1, min_ignore_avant, "plus petit avant from, à ignorer"),
    TL_TEST(1, min_une_case, "zone d'une seule case"),
    TL_TEST(1, min_a_la_fin, "minimum dans la dernière case"),
    TL_TEST(1, min_egalites, "minimum présent deux fois"),
    TL_TEST(1, min_extremes, "INT_MIN et INT_MAX"),
    TL_TEST(1, min_aleatoire, "1000 éléments, from = 300"),

    TL_SORT_SUITE_ENTRIES(2, selection_sort),
    TL_TEST(2, sort_min_au_bout, "minimum en dernière position"),
};

TL_MAIN("02", "Tri par sélection", STEPS, TESTS)

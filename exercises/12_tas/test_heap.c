/* Exercice 12 — tests. NE PAS MODIFIER CE FICHIER. */
#include "heap.h"
#include "tl_check.h"

/* Vrai si la case j appartient au sous-arbre de racine i. */
static bool in_subtree(size_t j, size_t i)
{
    while (j > i) {
        j = (j - 1) / 2;
    }
    return j == i;
}

/* Premier indice c du sous-arbre de i (c > i) plus grand que son parent, ou n. */
static size_t subtree_violation(const int *a, size_t n, size_t i)
{
    for (size_t c = i + 1; c < n; c++) {
        if (in_subtree(c, i) && a[(c - 1) / 2] < a[c]) {
            return c;
        }
    }
    return n;
}

/* ---------------- Étape 1 · sift_down ---------------- */

/* in a size cases ; sift_down travaille sur les n premières. */
static void check_sift(const int *in, size_t size, size_t n, size_t i, const int *expected)
{
    int *a = tl_ints_copy(in, size);
    tl_on_crash("Avant de lire un enfant, vérifies-tu que son indice est < n ?");
    tl_on_timeout("Après chaque échange, ton indice descend-il vers les feuilles ? Quand "
                  "t'arrêtes-tu ?");
    sift_down(a, n, i);
    tl_check_guards(a, size, "sift_down");
    if (tl_ints_equal(a, expected, size)) {
        tl_ints_free(a);
        return;
    }
    tl_line("sift_down(a, n = %zu, i = %zu)", n, i);
    tl_report_ints(in, expected, a, size, NULL, NULL);
    if (tl_ints_equal(a, in, size)) {
        tl_symptom("le tableau n'a pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : compares-tu "
                    "a[i] à ses enfants, aux cases 2i + 1 et 2i + 2 ?");
        tl_fail();
    }
    for (size_t j = 0; j < size; j++) {
        if (a[j] != in[j] && (j >= n || !in_subtree(j, i))) {
            if (j >= n) {
                tl_symptom("la case a[%zu] est au-delà de n = %zu, et elle a changé.", j, n);
                tl_question("Vérifies-tu qu'un enfant existe (indice < n) avant de le "
                            "comparer ?");
            } else {
                tl_symptom("la case a[%zu] n'est pas dans le sous-arbre de %zu, et elle a "
                           "changé.", j, i);
                tl_question("Quels sont les enfants de la case i ? Comment les calcules-tu ?");
            }
            tl_fail();
        }
    }
    tl_perm p = tl_check_permutation(in, a, size);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Que contient *x juste après la première affectation de ton swap ?");
        tl_fail();
    }
    size_t c = subtree_violation(a, n, i);
    if (c < n) {
        tl_symptom("a[%zu] = %d est plus grand que son parent a[%zu] = %d.", c, a[c],
                   (c - 1) / 2, a[(c - 1) / 2]);
        tl_question("Échanges-tu avec le plus grand des deux enfants ? Après un échange, "
                    "continues-tu depuis la nouvelle case de la valeur ?");
    } else {
        tl_symptom("le sous-arbre est bien un tas, mais pas celui qu'on attend.");
        tl_question("Quand les deux enfants sont égaux, lequel choisis-tu ? Échanges-tu "
                    "quand la valeur est égale à son plus grand enfant ?");
    }
    tl_fail();
}

static void sift_racine(void)
{
    static const int in[] = { 1, 5, 3 };
    static const int expected[] = { 5, 1, 3 };
    check_sift(in, 3, 3, 0, expected);
}

static void sift_deux_niveaux(void)
{
    static const int in[] = { 2, 9, 7, 4, 8, 6, 5 };
    static const int expected[] = { 9, 8, 7, 4, 2, 6, 5 };
    check_sift(in, 7, 7, 0, expected);
}

static void sift_profond(void)
{
    static const int in[] = { 0, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    static const int expected[] = { 14, 12, 13, 8, 11, 10, 9, 0, 7, 6, 5, 4, 3, 2, 1 };
    check_sift(in, 15, 15, 0, expected);
}

static void sift_deja_tas(void)
{
    static const int in[] = { 9, 5, 7 };
    check_sift(in, 3, 3, 0, in);
}

static void sift_feuille(void)
{
    static const int in[] = { 1, 2, 3 };
    check_sift(in, 3, 3, 2, in);
}

static void sift_un_enfant(void)
{
    static const int in[] = { 1, 2 };
    static const int expected[] = { 2, 1 };
    check_sift(in, 2, 2, 0, expected);
}

static void sift_sous_arbre(void)
{
    static const int in[] = { 0, 1, 9, 5, 3, 8, 7 };
    static const int expected[] = { 0, 5, 9, 1, 3, 8, 7 };
    check_sift(in, 7, 7, 1, expected);
}

static void sift_ignore_fin(void)
{
    static const int in[] = { 1, 2, 3, 9 };
    static const int expected[] = { 3, 2, 1, 9 };
    check_sift(in, 4, 3, 0, expected);
}

static void sift_enfants_egaux(void)
{
    static const int in[] = { 1, 4, 4 };
    static const int expected[] = { 4, 1, 4 };
    check_sift(in, 3, 3, 0, expected);
}

static void sift_egal_enfant(void)
{
    static const int in[] = { 4, 4, 1 };
    check_sift(in, 3, 3, 0, in);
}

static void sift_extremes(void)
{
    static const int in[] = { INT_MIN, INT_MAX, 0 };
    static const int expected[] = { INT_MAX, INT_MIN, 0 };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_sift(in, 3, 3, 0, expected);
}

/* ---------------- Étape 2 · build_max_heap ---------------- */

static void check_build(const int *in, size_t n)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta boucle tourne-t-elle quand même ? Que vaut n / 2 - 1 "
                    "avec des size_t ?");
        build_max_heap(NULL, 0);
        return;
    }
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle descend-elle jusqu'à la case 0 sans passer sous 0 ? Avec des "
                "size_t, que vaut 0 - 1 ?");
    tl_on_timeout("Ta boucle descend-elle jusqu'à la case 0 sans passer sous 0 ? Avec des "
                  "size_t, que vaut 0 - 1 ?");
    build_max_heap(a, n);
    tl_check_guards(a, n, "build_max_heap");
    size_t c = tl_first_heap_violation(a, n);
    tl_perm p = tl_check_permutation(in, a, n);
    if (c == n && p.ok) {
        tl_ints_free(a);
        return;
    }
    if (n <= TL_SHOW_MAX) {
        tl_show("entrée", in, n, 0, NULL);
    }
    tl_show("obtenu", a, n, c < n ? c : 0, NULL);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Tes tests de sift_down passent-ils tous ?");
    } else if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé, et ce n'est pas un tas : a[%zu] = %d est plus "
                   "grand que son parent a[%zu] = %d.", c, a[c], (c - 1) / 2, a[(c - 1) / 2]);
        tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : sur quelles "
                    "cases appelles-tu sift_down ?");
    } else {
        tl_symptom("a[%zu] = %d est plus grand que son parent a[%zu] = %d.", c, a[c],
                   (c - 1) / 2, a[(c - 1) / 2]);
        tl_question("sift_down suppose que les sous-arbres des enfants sont déjà des tas : "
                    "dans quel ordre dois-tu parcourir les cases pour que ce soit vrai ? "
                    "Vas-tu jusqu'à la racine ?");
    }
    tl_fail();
}

static void build_vide(void)
{
    check_build(NULL, 0);
}

static void build_un(void)
{
    static const int in[] = { 3 };
    check_build(in, 1);
}

static void build_croissant(void)
{
    static const int in[] = { 1, 2, 3, 4, 5, 6, 7 };
    check_build(in, 7);
}

static void build_pair(void)
{
    static const int in[] = { 1, 2, 3, 4, 5, 6 };
    check_build(in, 6);
}

static void build_doublons(void)
{
    static const int in[] = { 2, 0, 3, 3, 1, 0, 2, 3, 1 };
    check_build(in, 9);
}

static void build_extremes(void)
{
    static const int in[] = { INT_MIN, 0, INT_MAX, -1, INT_MIN, INT_MAX };
    check_build(in, 6);
}

static void build_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1212);
    tl_fill_random(in, N, -1000, 1000);
    check_build(in, N);
    tl_ints_free(in);
}

static void build_grand(void)
{
    enum { N = 1000000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1213);
    tl_fill_random(in, N, INT_MIN, INT_MAX);
    check_build(in, N);
    tl_ints_free(in);
}

/* ---------------- Étape 3 · heap_sort ---------------- */

TL_SORT_SUITE_DEFINE(heap_sort, 200000)

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "sift_down", false },
    { 2, "build_max_heap", false },
    { 3, "heap_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, sift_racine, "{1, 5, 3}, depuis la racine"),
    TL_TEST(1, sift_deux_niveaux, "descente sur deux niveaux"),
    TL_TEST(1, sift_profond, "descente jusqu'à une feuille, 15 cases"),
    TL_TEST(1, sift_deja_tas, "déjà un tas"),
    TL_TEST(1, sift_feuille, "depuis une feuille"),
    TL_TEST(1, sift_un_enfant, "un seul enfant"),
    TL_TEST(1, sift_sous_arbre, "sous-arbre de la case 1"),
    TL_TEST(1, sift_ignore_fin, "la case a[3] est hors du tas (n = 3)"),
    TL_TEST(1, sift_enfants_egaux, "deux enfants égaux"),
    TL_TEST(1, sift_egal_enfant, "valeur égale à son plus grand enfant"),
    TL_TEST(1, sift_extremes, "INT_MIN et INT_MAX"),

    TL_TEST(2, build_vide, "n = 0 avec a = NULL"),
    TL_TEST(2, build_un, "1 élément"),
    TL_TEST(2, build_croissant, "{1, 2, 3, 4, 5, 6, 7}"),
    TL_TEST(2, build_pair, "nombre pair d'éléments"),
    TL_TEST(2, build_doublons, "doublons"),
    TL_TEST(2, build_extremes, "INT_MIN et INT_MAX"),
    TL_TEST(2, build_aleatoire, "1000 éléments aléatoires"),
    TL_TEST(2, build_grand, "1 000 000 éléments aléatoires"),

    TL_SORT_SUITE_ENTRIES(3, heap_sort),
};

TL_MAIN("12", "Tri par tas", STEPS, TESTS)

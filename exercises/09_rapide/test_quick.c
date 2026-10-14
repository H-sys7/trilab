/* Exercice 09 — tests. NE PAS MODIFIER CE FICHIER. */
#include "quick.h"
#include "tl_check.h"

/* Vérifie qu'aucune case hors de a[lo..hi) n'a changé. */
static void check_zone_only(const int *in, const int *a, size_t n, size_t lo, size_t hi)
{
    for (size_t i = 0; i < n; i++) {
        if ((i < lo || i >= hi) && a[i] != in[i]) {
            tl_symptom("la case a[%zu] est hors de la zone a[%zu..%zu), et elle a changé.", i,
                       lo, hi);
            tl_question("Quelles cases ta fonction a-t-elle le droit de toucher ?");
            tl_fail();
        }
    }
}

static void check_permutation_zone(const int *in, const int *a, size_t n)
{
    tl_perm p = tl_check_permutation(in, a, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Que contient *x juste après la première affectation de ton swap ?");
        tl_fail();
    }
}

/* ---------------- Étape 1 · partition_last ---------------- */

static void check_partition(const int *in, size_t n, size_t lo, size_t hi, size_t expected_p)
{
    int v = in[hi - 1];
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle lit-elle une case en dehors de a[lo..hi) ? Que vaut ton indice "
                "quand lo vaut 0 ?");
    size_t p = partition_last(a, lo, hi);
    tl_check_guards(a, n, "partition_last");
    tl_line("zone    : a[%zu..%zu), pivot v = %d", lo, hi, v);
    if (n <= TL_SHOW_MAX) {
        tl_show("entrée", in, n, lo, NULL);
    }
    char note[48];
    snprintf(note, sizeof note, "(p = %zu)", p);
    if (p < lo || p >= hi) {
        tl_show("obtenu", a, n, lo, note);
        if (p == 0 && tl_ints_equal(a, in, n)) {
            tl_symptom("le tableau n'a pas changé et p vaut 0, en dehors de la zone.");
            tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : que doit "
                        "renvoyer ta fonction ?");
        } else {
            tl_symptom("p = %zu est en dehors de la zone a[%zu..%zu).", p, lo, hi);
            tl_question("Où se trouve le pivot à la fin de la partition, et renvoies-tu bien "
                        "cet indice ?");
        }
        tl_fail();
    }
    tl_show("obtenu", a, n, p, note);
    if (p == 0 && expected_p != 0 && tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé, et p vaut %zu.", p);
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : ta boucle "
                    "déplace-t-elle les valeurs < v vers le début de la zone ?");
        tl_fail();
    }
    check_zone_only(in, a, n, lo, hi);
    check_permutation_zone(in, a, n);
    if (a[p] != v) {
        tl_symptom("a[p] = a[%zu] vaut %d, et non le pivot %d.", p, a[p], v);
        tl_question("À la fin, remets-tu le pivot à sa place définitive, entre les deux "
                    "parties ?");
        tl_fail();
    }
    for (size_t i = lo; i < p; i++) {
        if (a[i] >= v) {
            tl_symptom("a[%zu] = %d est à gauche du pivot, alors qu'elle n'est pas < %d.", i,
                       a[i], v);
            tl_question("Quelles valeurs ta boucle envoie-t-elle à gauche : les < v, ou "
                        "aussi les égales ?");
            tl_fail();
        }
    }
    for (size_t i = p + 1; i < hi; i++) {
        if (a[i] < v) {
            tl_symptom("a[%zu] = %d est à droite du pivot, alors qu'elle est < %d.", i, a[i],
                       v);
            tl_question("Ta boucle examine-t-elle toutes les cases de a[lo..hi - 1) ?");
            tl_fail();
        }
    }
    if (p != expected_p) {
        tl_symptom("p vaut %zu au lieu de %zu.", p, expected_p);
        tl_question("Combien de valeurs de la zone sont strictement plus petites que v ?");
        tl_fail();
    }
    tl_ints_free(a);
}

static void part_exemple(void)
{
    static const int in[] = { 3, 8, 1, 9, 4, 5 };
    check_partition(in, 6, 0, 6, 3);
}

static void part_un(void)
{
    static const int in[] = { 5 };
    check_partition(in, 1, 0, 1, 0);
}

static void part_deux(void)
{
    static const int in[] = { 2, 1 };
    check_partition(in, 2, 0, 2, 0);
}

static void part_pivot_min(void)
{
    static const int in[] = { 4, 7, 2, 9, 1 };
    check_partition(in, 5, 0, 5, 0);
}

static void part_pivot_max(void)
{
    static const int in[] = { 4, 7, 2, 1, 9 };
    check_partition(in, 5, 0, 5, 4);
}

static void part_egaux(void)
{
    static const int in[] = { 3, 3, 3 };
    check_partition(in, 3, 0, 3, 0);
}

static void part_doublons(void)
{
    static const int in[] = { 5, 1, 5, 2, 5 };
    check_partition(in, 5, 0, 5, 2);
}

static void part_zone(void)
{
    static const int in[] = { 100, 6, 2, 8, 4, -100 };
    check_partition(in, 6, 1, 5, 2);
}

static void part_extremes(void)
{
    static const int in[] = { INT_MAX, INT_MIN, 0, -1 };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_partition(in, 4, 0, 4, 1);
}

static void part_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(9009);
    tl_fill_random(in, N, -500, 500);
    size_t smaller = 0;
    for (size_t i = 0; i + 1 < N; i++) {
        if (in[i] < in[N - 1]) {
            smaller++;
        }
    }
    check_partition(in, N, 0, N, smaller);
    tl_ints_free(in);
}

/* ---------------- Étape 2 · quick_sort_naive ---------------- */

TL_SORT_SUITE_DEFINE(quick_sort_naive, 100000)

/* ---------------- Étape 3 · median_of_three ---------------- */

static int median3(int x, int y, int z)
{
    int t[3] = { x, y, z };
    qsort(t, 3, sizeof(int), tl_cmp_int);
    return t[1];
}

/* Renvoie false (après avoir affiché le diagnostic) si le résultat est faux. */
static bool check_median(const int *in, size_t n, size_t lo, size_t hi)
{
    size_t mid = lo + (hi - lo) / 2;
    int m = median3(in[lo], in[mid], in[hi - 1]);
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Quelles cases ta fonction lit-elle ? Sont-elles toutes dans a[lo..hi) ?");
    median_of_three(a, lo, hi);
    tl_check_guards(a, n, "median_of_three");
    bool ok = a[hi - 1] == m;
    int got3[3] = { a[lo], a[mid], a[hi - 1] };
    int in3[3] = { in[lo], in[mid], in[hi - 1] };
    qsort(got3, 3, sizeof(int), tl_cmp_int);
    qsort(in3, 3, sizeof(int), tl_cmp_int);
    bool same3 = tl_ints_equal(got3, in3, 3);
    size_t other = n;
    for (size_t i = 0; i < n; i++) {
        if (i != lo && i != mid && i != hi - 1 && a[i] != in[i]) {
            other = i;
            break;
        }
    }
    if (ok && same3 && other == n) {
        tl_ints_free(a);
        return true;
    }
    tl_line("zone    : a[%zu..%zu), mid = %zu", lo, hi, mid);
    tl_line("valeurs : a[lo] = %d, a[mid] = %d, a[hi - 1] = %d : médiane %d", in[lo], in[mid],
            in[hi - 1], m);
    tl_show("entrée", in, n, lo, NULL);
    tl_show("obtenu", a, n, lo, NULL);
    if (other < n) {
        tl_symptom("la case a[%zu] ne fait pas partie des trois cases, et elle a changé.",
                   other);
        tl_question("Quelles sont les trois seules cases que ta fonction a le droit de "
                    "toucher ? Comment calcules-tu mid ?");
    } else if (!same3) {
        tl_symptom("les trois cases ne contiennent plus les trois valeurs de départ.");
        tl_question("Tes échanges gardent-ils les deux valeurs échangées ?");
    } else if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé, et a[hi - 1] = %d n'est pas la médiane.",
                   a[hi - 1]);
        tl_question("Si tu n'as pas commencé l'étape 3, c'est normal. Sinon : quelle case "
                    "doit recevoir la médiane ?");
    } else {
        tl_symptom("a[hi - 1] = %d, mais la médiane des trois valeurs est %d.", a[hi - 1], m);
        tl_question("Parmi les trois valeurs, laquelle n'est ni la plus petite ni la plus "
                    "grande ? Où ta fonction la place-t-elle ?");
    }
    return false;
}

static void med_six_ordres(void)
{
    static const int orders[6][3] = {
        { 1, 2, 3 }, { 1, 3, 2 }, { 2, 1, 3 }, { 2, 3, 1 }, { 3, 1, 2 }, { 3, 2, 1 },
    };
    for (size_t k = 0; k < 6; k++) {
        if (!check_median(orders[k], 3, 0, 3)) {
            tl_fail();
        }
    }
}

static void med_doublons(void)
{
    static const int in1[] = { 2, 2, 1 };
    static const int in2[] = { 1, 2, 1 };
    static const int in3[] = { 5, 5, 5 };
    if (!check_median(in1, 3, 0, 3) || !check_median(in2, 3, 0, 3)
        || !check_median(in3, 3, 0, 3)) {
        tl_fail();
    }
}

static void med_grande_zone(void)
{
    static const int in[] = { 40, 11, 12, 10, 13, 14, 20 };
    if (!check_median(in, 7, 0, 7)) {
        tl_fail();
    }
}

static void med_zone(void)
{
    static const int in[] = { -1, -2, 9, 0, 1, 7, 2, 3, 8 };
    if (!check_median(in, 9, 2, 9)) {
        tl_fail();
    }
}

static void med_extremes(void)
{
    static const int in[] = { INT_MAX, INT_MIN, 0 };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    if (!check_median(in, 3, 0, 3)) {
        tl_fail();
    }
}

/* ---------------- Étape 4 · quick_sort ---------------- */

TL_SORT_SUITE_DEFINE(quick_sort, 200000)

static void sort_ordered(int sign)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = sign * (int)i;
    }
    tl_on_timeout("Choisis-tu le pivot avec median_of_three avant chaque partition d'une zone "
                  "d'au moins 3 éléments ?");
    tl_on_crash("La pile d'appels a débordé : la récursion est trop profonde. Choisis-tu le "
                "pivot avec median_of_three avant chaque partition ?");
    tl_check_sort_int("quick_sort", quick_sort, in, N, NULL);
    tl_ints_free(in);
}

static void sort_deja_trie_grand(void)
{
    sort_ordered(1);
}

static void sort_inverse_grand(void)
{
    sort_ordered(-1);
}

/* ---------------- Bonus 1 · partition_hoare ---------------- */

static void check_hoare(const int *in, size_t n, size_t lo, size_t hi)
{
    int v = in[lo];
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Tes deux indices peuvent-ils sortir de la zone avant de se croiser ?");
    tl_on_timeout("Tes deux indices avancent-ils à chaque tour, même sur des valeurs égales "
                  "au pivot ?");
    size_t j = partition_hoare(a, lo, hi);
    tl_check_guards(a, n, "partition_hoare");
    tl_line("zone    : a[%zu..%zu), pivot v = %d", lo, hi, v);
    if (n <= TL_SHOW_MAX) {
        tl_show("entrée", in, n, lo, NULL);
        tl_show("obtenu", a, n, lo, NULL);
    }
    if (j < lo || j + 1 >= hi) {
        tl_symptom("j = %zu, alors qu'il doit vérifier %zu <= j < %zu.", j, lo, hi - 1);
        tl_question("Si tu n'as pas commencé le bonus, c'est normal. Sinon : quel indice "
                    "renvoies-tu quand les deux indices se croisent ?");
        tl_fail();
    }
    check_zone_only(in, a, n, lo, hi);
    check_permutation_zone(in, a, n);
    for (size_t i = lo; i < hi; i++) {
        if ((i <= j && a[i] > v) || (i > j && a[i] < v)) {
            tl_symptom("j = %zu, mais a[%zu] = %d est du mauvais côté du pivot %d.", j, i, a[i],
                       v);
            tl_question("Quand tes deux indices s'arrêtent, échanges-tu les deux valeurs "
                        "avant de repartir ?");
            tl_fail();
        }
    }
    tl_ints_free(a);
}

static void hoare_exemple(void)
{
    static const int in[] = { 5, 3, 8, 1, 9, 2 };
    check_hoare(in, 6, 0, 6);
}

static void hoare_deux(void)
{
    static const int in[] = { 2, 1 };
    check_hoare(in, 2, 0, 2);
}

static void hoare_pivot_min(void)
{
    static const int in[] = { 1, 2, 3, 4 };
    check_hoare(in, 4, 0, 4);
}

static void hoare_pivot_max(void)
{
    static const int in[] = { 4, 3, 2, 1 };
    check_hoare(in, 4, 0, 4);
}

static void hoare_egaux(void)
{
    static const int in[] = { 2, 2, 2, 2, 2 };
    check_hoare(in, 5, 0, 5);
}

static void hoare_zone(void)
{
    static const int in[] = { 100, 6, 2, 8, 4, 7, -100 };
    check_hoare(in, 7, 1, 6);
}

static void hoare_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(9010);
    tl_fill_random(in, N, -20, 20);
    check_hoare(in, N, 0, N);
    tl_ints_free(in);
}

/* ---------------- Bonus 2 · partition_three_way ---------------- */

static void check_3way(const int *in, size_t n, size_t lo, size_t hi)
{
    int v = in[lo];
    size_t exp_lt = lo;
    size_t exp_gt = lo;
    for (size_t i = lo; i < hi; i++) {
        if (in[i] < v) {
            exp_lt++;
        }
        if (in[i] <= v) {
            exp_gt++;
        }
    }
    int *a = tl_ints_copy(in, n);
    size_t lt = (size_t)-1;
    size_t gt = (size_t)-1;
    tl_on_crash("Tes trois indices restent-ils dans a[lo..hi) ?");
    partition_three_way(a, lo, hi, &lt, &gt);
    tl_check_guards(a, n, "partition_three_way");
    tl_line("zone    : a[%zu..%zu), pivot v = %d", lo, hi, v);
    if (n <= TL_SHOW_MAX) {
        tl_show("entrée", in, n, lo, NULL);
        tl_show("obtenu", a, n, lo, NULL);
    }
    if (lt != exp_lt || gt != exp_gt) {
        tl_line("attendu : *lt = %zu, *gt = %zu", exp_lt, exp_gt);
        tl_line("obtenu  : *lt = %zu, *gt = %zu", lt, gt);
        if (lt == (size_t)-1 || gt == (size_t)-1) {
            tl_symptom("*lt ou *gt n'a pas été écrit.");
            tl_question("Si tu n'as pas commencé le bonus, c'est normal. Sinon : écris-tu "
                        "bien dans les variables pointées par lt et gt ?");
        } else {
            tl_symptom("les bornes des trois parties ne correspondent pas aux valeurs.");
            tl_question("Combien de valeurs de la zone sont < v ? Combien sont == v ?");
        }
        tl_fail();
    }
    check_zone_only(in, a, n, lo, hi);
    check_permutation_zone(in, a, n);
    for (size_t i = lo; i < hi; i++) {
        bool bad = (i < lt && a[i] >= v) || (i >= lt && i < gt && a[i] != v)
                   || (i >= gt && a[i] <= v);
        if (bad) {
            tl_symptom("a[%zu] = %d n'est pas dans la bonne partie (pivot %d).", i, a[i], v);
            tl_question("Après avoir échangé une valeur > v vers la fin, examines-tu la "
                        "valeur qui vient d'arriver ?");
            tl_fail();
        }
    }
    tl_ints_free(a);
}

static void trois_exemple(void)
{
    static const int in[] = { 3, 5, 3, 1, 3, 4, 2 };
    check_3way(in, 7, 0, 7);
}

static void trois_un(void)
{
    static const int in[] = { 1 };
    check_3way(in, 1, 0, 1);
}

static void trois_egaux(void)
{
    static const int in[] = { 7, 7, 7 };
    check_3way(in, 3, 0, 3);
}

static void trois_zone(void)
{
    static const int in[] = { 0, 4, 9, 4, 1, 4, 8 };
    check_3way(in, 7, 1, 6);
}

static void trois_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(9011);
    tl_fill_random(in, N, 0, 9);
    check_3way(in, N, 0, N);
    tl_ints_free(in);
}

/* ---------------- Bonus 3 · quick_sort_3way ---------------- */

TL_SORT_SUITE_DEFINE(quick_sort_3way, 200000)

static void sort3_tous_egaux_grand(void)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = 42;
    }
    tl_on_timeout("Les valeurs égales au pivot sont-elles exclues des appels récursifs ?");
    tl_on_crash("La pile d'appels a débordé : les valeurs égales au pivot sont-elles "
                "exclues des appels récursifs ?");
    tl_check_sort_int("quick_sort_3way", quick_sort_3way, in, N, NULL);
    tl_ints_free(in);
}

static void sort3_doublons_grand(void)
{
    tl_on_timeout("Les valeurs égales au pivot sont-elles exclues des appels récursifs ?");
    tl_check_sort_random("quick_sort_3way", quick_sort_3way, 200000, 9012, 0, 9);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "partition_last", false },
    { 2, "quick_sort_naive", false },
    { 3, "median_of_three", false },
    { 4, "quick_sort", false },
    { 5, "partition_hoare", true },
    { 6, "partition_three_way", true },
    { 7, "quick_sort_3way", true },
};

static const tl_test TESTS[] = {
    TL_TEST(1, part_exemple, "partition de {3, 8, 1, 9, 4, 5}"),
    TL_TEST(1, part_un, "zone d'un élément"),
    TL_TEST(1, part_deux, "partition de {2, 1}"),
    TL_TEST(1, part_pivot_min, "pivot plus petit que tout"),
    TL_TEST(1, part_pivot_max, "pivot plus grand que tout"),
    TL_TEST(1, part_egaux, "valeurs toutes égales"),
    TL_TEST(1, part_doublons, "plusieurs valeurs égales au pivot"),
    TL_TEST(1, part_zone, "zone a[1..5) de {100, 6, 2, 8, 4, -100}"),
    TL_TEST(1, part_extremes, "INT_MIN et INT_MAX"),
    TL_TEST(1, part_aleatoire, "1000 éléments aléatoires"),

    TL_SORT_SUITE_ENTRIES(2, quick_sort_naive),

    TL_TEST(3, med_six_ordres, "les 6 ordres de {1, 2, 3}"),
    TL_TEST(3, med_doublons, "valeurs égales"),
    TL_TEST(3, med_grande_zone, "zone de 7 éléments"),
    TL_TEST(3, med_zone, "zone a[2..9)"),
    TL_TEST(3, med_extremes, "INT_MAX, INT_MIN, 0"),

    TL_SORT_SUITE_ENTRIES(4, quick_sort),
    TL_TEST(4, sort_deja_trie_grand, "200 000 éléments déjà triés"),
    TL_TEST(4, sort_inverse_grand, "200 000 éléments triés à l'envers"),

    TL_TEST(5, hoare_exemple, "partition de {5, 3, 8, 1, 9, 2}"),
    TL_TEST(5, hoare_deux, "partition de {2, 1}"),
    TL_TEST(5, hoare_pivot_min, "pivot plus petit que tout"),
    TL_TEST(5, hoare_pivot_max, "pivot plus grand que tout"),
    TL_TEST(5, hoare_egaux, "valeurs toutes égales"),
    TL_TEST(5, hoare_zone, "zone a[1..6)"),
    TL_TEST(5, hoare_aleatoire, "1000 éléments aléatoires"),

    TL_TEST(6, trois_exemple, "partition de {3, 5, 3, 1, 3, 4, 2}"),
    TL_TEST(6, trois_un, "zone d'un élément"),
    TL_TEST(6, trois_egaux, "valeurs toutes égales"),
    TL_TEST(6, trois_zone, "zone a[1..6)"),
    TL_TEST(6, trois_aleatoire, "1000 valeurs entre 0 et 9"),

    TL_SORT_SUITE_ENTRIES(7, quick_sort_3way),
    TL_TEST(7, sort3_tous_egaux_grand, "200 000 valeurs égales"),
    TL_TEST(7, sort3_doublons_grand, "200 000 valeurs entre 0 et 9"),
};

TL_MAIN("09", "Tri rapide", STEPS, TESTS)

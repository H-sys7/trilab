/* Exercice 13 — tests. NE PAS MODIFIER CE FICHIER. */
#include "counting.h"
#include "tl_check.h"

#define SIZE_MARK ((size_t)424242) /* sentinelle placée après les tables de comptage */

static void show_sizes(const char *label, const size_t *c, size_t k)
{
    size_t w = tl_width(label, strlen(label));
    printf("    %s%*s: {", label, (int)(w < 8 ? 8 - w : 0), "");
    for (size_t i = 0; i < k; i++) {
        printf(i > 0 ? ", %zu" : "%zu", c[i]);
    }
    printf("}\n");
}

/* ---------------- Étape 1 · find_min_max ---------------- */

static void check_min_max(const int *in, size_t n, int exp_min, int exp_max)
{
    int *a = tl_ints_copy(in, n);
    int mn = 12345;
    int mx = 12345;
    tl_on_crash("Ta boucle lit-elle une case en dehors de a[0..n) ?");
    find_min_max(a, n, &mn, &mx);
    tl_check_guards(a, n, "find_min_max");
    if (mn == exp_min && mx == exp_max) {
        tl_ints_free(a);
        return;
    }
    tl_show("entrée", in, n, 0, NULL);
    tl_line("attendu : min = %d, max = %d", exp_min, exp_max);
    tl_line("obtenu  : min = %d, max = %d", mn, mx);
    bool found_min = false;
    bool found_max = false;
    for (size_t i = 0; i < n; i++) {
        found_min = found_min || in[i] == mn;
        found_max = found_max || in[i] == mx;
    }
    if (mn == 12345 && mx == 12345) {
        tl_symptom("*min et *max n'ont pas été écrits.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : écris-tu dans "
                    "les variables pointées par min et max ?");
    } else if (!found_min || !found_max) {
        tl_symptom("%d n'est même pas une valeur du tableau.", !found_min ? mn : mx);
        tl_question("Avec quelles valeurs commences-tu min et max ?");
    } else {
        tl_symptom("une valeur plus extrême n'a pas été vue.");
        tl_question("Ta boucle examine-t-elle toutes les cases, jusqu'à la dernière ?");
    }
    tl_fail();
}

static void mm_un(void)
{
    static const int in[] = { 5 };
    check_min_max(in, 1, 5, 5);
}

static void mm_exemple(void)
{
    static const int in[] = { 3, 1, 2 };
    check_min_max(in, 3, 1, 3);
}

static void mm_positifs(void)
{
    static const int in[] = { 5, 7, 6 };
    check_min_max(in, 3, 5, 7);
}

static void mm_negatifs(void)
{
    static const int in[] = { -5, -7, -6 };
    check_min_max(in, 3, -7, -5);
}

static void mm_aux_bouts(void)
{
    static const int in[] = { 9, 4, 6, 2 };
    check_min_max(in, 4, 2, 9);
}

static void mm_egaux(void)
{
    static const int in[] = { 7, 7, 7 };
    check_min_max(in, 3, 7, 7);
}

static void mm_extremes(void)
{
    static const int in[] = { 0, INT_MAX, INT_MIN, 1 };
    check_min_max(in, 4, INT_MIN, INT_MAX);
}

/* ---------------- Étape 2 · count_occurrences ---------------- */

static void check_count(const int *in, size_t n, int mn, const size_t *expected, size_t k)
{
    size_t *count = tl_malloc((k + 1) * sizeof *count);
    for (size_t i = 0; i < k; i++) {
        count[i] = 0;
    }
    count[k] = SIZE_MARK;
    tl_on_crash("Quelle case de count augmentes-tu pour la valeur v ? Cet indice est-il "
                "toujours entre 0 et k - 1 ?");
    count_occurrences(in, n, mn, count);
    bool ok = count[k] == SIZE_MARK;
    for (size_t i = 0; i < k && ok; i++) {
        ok = count[i] == expected[i];
    }
    if (ok) {
        tl_free(count);
        return;
    }
    tl_show("entrée", in, n, 0, NULL);
    tl_line("min     : %d, k = %zu cases", mn, k);
    show_sizes("attendu", expected, k);
    show_sizes("obtenu", count, k);
    if (count[k] != SIZE_MARK) {
        tl_symptom("la case count[%zu], juste après la table, a été modifiée.", k);
        tl_question("Pour la plus grande valeur, quel indice de count calcules-tu ?");
    } else {
        bool zero = true;
        for (size_t i = 0; i < k; i++) {
            zero = zero && count[i] == 0;
        }
        if (zero) {
            tl_symptom("la table est restée à zéro.");
            tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : quelle "
                        "case augmentes-tu pour chaque valeur ?");
        } else {
            tl_symptom("les comptes ne correspondent pas aux valeurs.");
            tl_question("La valeur min doit aller dans count[0] : quel indice calcules-tu "
                        "pour une valeur v ?");
        }
    }
    tl_fail();
}

static void cnt_exemple(void)
{
    static const int in[] = { 3, 1, 3, 2, 1, 3 };
    static const size_t expected[] = { 2, 1, 3 };
    check_count(in, 6, 1, expected, 3);
}

static void cnt_negatifs(void)
{
    static const int in[] = { -2, 0, -2 };
    static const size_t expected[] = { 2, 0, 1 };
    check_count(in, 3, -2, expected, 3);
}

static void cnt_un(void)
{
    static const int in[] = { 5 };
    static const size_t expected[] = { 1 };
    check_count(in, 1, 5, expected, 1);
}

static void cnt_trous(void)
{
    static const int in[] = { 10, 14, 10 };
    static const size_t expected[] = { 2, 0, 0, 0, 1 };
    check_count(in, 3, 10, expected, 5);
}

/* ---------------- Étape 3 · prefix_sums ---------------- */

static void check_prefix(const size_t *in, size_t k, const size_t *expected)
{
    size_t *c = tl_malloc((k + 1) * sizeof *c);
    memcpy(c, in, k * sizeof *c);
    c[k] = SIZE_MARK;
    tl_on_crash("Ta boucle lit-elle ou écrit-elle en dehors de count[0..k) ?");
    prefix_sums(c, k);
    bool ok = c[k] == SIZE_MARK;
    for (size_t i = 0; i < k && ok; i++) {
        ok = c[i] == expected[i];
    }
    if (ok) {
        tl_free(c);
        return;
    }
    show_sizes("entrée", in, k);
    show_sizes("attendu", expected, k);
    show_sizes("obtenu", c, k);
    bool same = true;
    bool inclusive = true;
    size_t sum = 0;
    for (size_t i = 0; i < k; i++) {
        same = same && c[i] == in[i];
        sum += in[i];
        inclusive = inclusive && c[i] == sum;
    }
    if (c[k] != SIZE_MARK) {
        tl_symptom("la case juste après la table a été modifiée.");
        tl_question("Pour k cases, quel est le dernier indice valide ?");
    } else if (same) {
        tl_symptom("la table n'a pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 3, c'est normal. Sinon : que doit "
                    "contenir count[i] après l'appel ?");
    } else if (inclusive) {
        tl_symptom("chaque case contient la somme jusqu'à elle-même incluse.");
        tl_question("count[i] doit-il compter sa propre valeur, ou seulement celles des "
                    "cases d'avant ? Que se passe-t-il si tu écrases count[i] avant de "
                    "l'avoir utilisé ?");
    } else {
        tl_symptom("les sommes ne correspondent pas.");
        tl_question("Déroule ta boucle sur {2, 1, 3} : quelle somme gardes-tu de côté à "
                    "chaque tour, et quand l'écris-tu ?");
    }
    tl_fail();
}

static void pre_exemple(void)
{
    static const size_t in[] = { 2, 1, 3 };
    static const size_t expected[] = { 0, 2, 3 };
    check_prefix(in, 3, expected);
}

static void pre_un(void)
{
    static const size_t in[] = { 4 };
    static const size_t expected[] = { 0 };
    check_prefix(in, 1, expected);
}

static void pre_zeros(void)
{
    static const size_t in[] = { 4, 0, 0, 1 };
    static const size_t expected[] = { 0, 4, 4, 4 };
    check_prefix(in, 4, expected);
}

static void pre_fin(void)
{
    static const size_t in[] = { 0, 0, 5 };
    static const size_t expected[] = { 0, 0, 0 };
    check_prefix(in, 3, expected);
}

/* ---------------- Étape 4 · counting_sort ---------------- */

/* Tri qui doit réussir (k raisonnable) : un code -1 est une erreur. */
static void counting_ok(int *a, size_t n)
{
    int ret = counting_sort(a, n);
    if (ret != 0) {
        tl_line("%zu valeurs, écart max - min raisonnable", n);
        tl_symptom("counting_sort renvoie %d alors qu'il devait réussir.", ret);
        tl_question("Comment calcules-tu k ? Le compares-tu bien à COUNTING_MAX_RANGE ?");
        tl_fail();
    }
}

static void check_sorts(const int *in, size_t n)
{
    tl_on_crash("Calcules-tu k = max - min + 1 sans dépasser les limites d'un int ? Les "
                "indices dans count restent-ils dans [0, k) ?");
    tl_on_ub("Ton calcul de max - min peut-il dépasser les limites d'un int ?");
    tl_check_sort_int("counting_sort", counting_ok, in, n, NULL);
}

static void sort_vide(void)
{
    check_sorts(NULL, 0);
}

static void sort_un(void)
{
    static const int in[] = { 42 };
    check_sorts(in, 1);
}

static void sort_deux(void)
{
    static const int in[] = { 2, 1 };
    check_sorts(in, 2);
}

static void sort_trie(void)
{
    static const int in[] = { -3, 0, 2, 5, 8, 13, 21, 34, 55, 89 };
    check_sorts(in, 10);
}

static void sort_inverse(void)
{
    static const int in[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    check_sorts(in, 10);
}

static void sort_doublons(void)
{
    static const int in[] = { 5, 3, 5, 1, 3, 5, 0, 1 };
    check_sorts(in, 8);
}

static void sort_egaux(void)
{
    static const int in[] = { 7, 7, 7, 7, 7, 7 };
    check_sorts(in, 6);
}

static void sort_negatifs(void)
{
    static const int in[] = { -3, 5, -3, 0, -1, -1000000 };
    check_sorts(in, 6);
}

static void sort_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1313);
    tl_fill_random(in, N, -1000000, 1000000);
    check_sorts(in, N);
    tl_ints_free(in);
}

static void sort_grand(void)
{
    enum { N = 1000000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1314);
    tl_fill_random(in, N, 0, 1000000);
    check_sorts(in, N);
    tl_ints_free(in);
}

static void sort_plage_limite(void)
{
    static const int in[] = { 9999999, 0, 5 };
    check_sorts(in, 3);
}

static void check_refused(const int *in, size_t n)
{
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Calcules-tu k = max - min + 1 sans dépasser les limites d'un int ?");
    tl_on_ub("Ton calcul de max - min peut-il dépasser les limites d'un int ? Dans quel "
             "type le fais-tu ?");
    int ret = counting_sort(a, n);
    tl_check_guards(a, n, "counting_sort");
    if (ret == -1 && tl_ints_equal(a, in, n)) {
        tl_ints_free(a);
        return;
    }
    tl_show("entrée", in, n, 0, NULL);
    tl_line("attendu : renvoie -1, tableau inchangé (k dépasse COUNTING_MAX_RANGE)");
    tl_show("obtenu", a, n, 0, NULL);
    tl_line("valeur renvoyée : %d", ret);
    if (ret != -1) {
        tl_symptom("la table serait trop grande, mais la fonction ne le signale pas.");
        tl_question("Compares-tu k à COUNTING_MAX_RANGE avant d'allouer ? Dans quel type "
                    "calcules-tu k ?");
    } else {
        tl_symptom("la fonction renvoie -1, mais le tableau a été modifié.");
        tl_question("À quel moment fais-tu la vérification : avant ou après avoir touché "
                    "au tableau ?");
    }
    tl_fail();
}

static void refus_plage(void)
{
    static const int in[] = { 0, 10000000, 3 };
    check_refused(in, 3);
}

static void refus_extremes(void)
{
    static const int in[] = { INT_MAX, 0, INT_MIN };
    check_refused(in, 3);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "find_min_max", false },
    { 2, "count_occurrences", false },
    { 3, "prefix_sums", false },
    { 4, "counting_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, mm_un, "1 élément"),
    TL_TEST(1, mm_exemple, "{3, 1, 2}"),
    TL_TEST(1, mm_positifs, "{5, 7, 6}"),
    TL_TEST(1, mm_negatifs, "{-5, -7, -6}"),
    TL_TEST(1, mm_aux_bouts, "max au début, min à la fin"),
    TL_TEST(1, mm_egaux, "tous égaux"),
    TL_TEST(1, mm_extremes, "INT_MIN et INT_MAX"),

    TL_TEST(2, cnt_exemple, "{3, 1, 3, 2, 1, 3}, min = 1"),
    TL_TEST(2, cnt_negatifs, "{-2, 0, -2}, min = -2"),
    TL_TEST(2, cnt_un, "{5}, min = 5"),
    TL_TEST(2, cnt_trous, "valeurs absentes entre min et max"),

    TL_TEST(3, pre_exemple, "{2, 1, 3}"),
    TL_TEST(3, pre_un, "une case"),
    TL_TEST(3, pre_zeros, "{4, 0, 0, 1}"),
    TL_TEST(3, pre_fin, "{0, 0, 5}"),

    TL_TEST(4, sort_vide, "n = 0 avec a = NULL"),
    TL_TEST(4, sort_un, "1 élément"),
    TL_TEST(4, sort_deux, "2 éléments inversés"),
    TL_TEST(4, sort_trie, "déjà trié"),
    TL_TEST(4, sort_inverse, "trié à l'envers"),
    TL_TEST(4, sort_doublons, "doublons"),
    TL_TEST(4, sort_egaux, "tous égaux"),
    TL_TEST(4, sort_negatifs, "valeurs négatives"),
    TL_TEST(4, sort_aleatoire, "1000 valeurs entre -1 000 000 et 1 000 000"),
    TL_TEST(4, sort_grand, "1 000 000 valeurs entre 0 et 1 000 000"),
    TL_TEST(4, sort_plage_limite, "k = COUNTING_MAX_RANGE exactement"),
    TL_TEST(4, refus_plage, "k = COUNTING_MAX_RANGE + 1 : refusé"),
    TL_TEST(4, refus_extremes, "INT_MIN et INT_MAX : refusé"),
};

TL_MAIN("13", "Tri par comptage", STEPS, TESTS)

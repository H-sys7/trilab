/* Exercice 18 — tests. NE PAS MODIFIER CE FICHIER. */
#include "tim.h"
#include "tl_check.h"

/* ---------------- Étape 1 · count_run ---------------- */

static void check_run(const int *in, size_t n, size_t lo, size_t hi, size_t exp_len,
                      const int *expected)
{
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle s'arrête-t-elle avant hi ? Lis-tu a[i + 1] quand i + 1 == hi ?");
    size_t len = count_run(a, lo, hi);
    tl_check_guards(a, n, "count_run");
    if (len == exp_len && tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }
    char note_exp[48];
    char note_got[48];
    snprintf(note_exp, sizeof note_exp, "(longueur %zu)", exp_len);
    snprintf(note_got, sizeof note_got, "(longueur %zu)", len);
    tl_line("zone    : a[%zu..%zu)", lo, hi);
    tl_report_ints(in, expected, a, n, note_exp, note_got);
    bool changed_outside = false;
    for (size_t i = 0; i < n; i++) {
        changed_outside = changed_outside || ((i < lo || i >= lo + exp_len) && a[i] != in[i]);
    }
    if (len == 0) {
        tl_symptom("la fonction renvoie 0, alors qu'une suite a toujours au moins 1 élément.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : que renvoies-tu "
                    "quand la zone n'a qu'un élément ?");
    } else if (len > exp_len) {
        tl_symptom("la suite mesurée est trop longue.");
        tl_question("Une suite strictement décroissante s'arrête-t-elle sur deux valeurs "
                    "égales (sinon, les retourner casserait la stabilité) ? Une suite "
                    "croissante peut-elle commencer par deux valeurs égales ? Ta boucle "
                    "s'arrête-t-elle à hi ?");
    } else if (changed_outside) {
        tl_symptom("une case hors de la suite a été modifiée.");
        tl_question("Quand tu retournes la suite, sur quelles cases travailles-tu "
                    "exactement ?");
    } else if (len == exp_len && tl_ints_equal(a, in, n)) {
        tl_symptom("la bonne longueur, mais la suite décroissante n'a pas été retournée.");
        tl_question("Après avoir mesuré une suite strictement décroissante, la "
                    "retournes-tu sur place ?");
    } else if (len != exp_len) {
        tl_symptom("la longueur renvoyée est %zu au lieu de %zu.", len, exp_len);
        tl_question("Comptes-tu bien le premier élément ? Comparer a[i] à a[i + 1] : "
                    "quand t'arrêtes-tu ?");
    } else {
        tl_symptom_diff(in, expected, a, n);
        tl_question("Retournes-tu exactement les cases a[lo..lo + longueur) ?");
    }
    tl_fail();
}

static void run_croissante(void)
{
    static const int in[] = { 1, 2, 3, 0 };
    check_run(in, 4, 0, 4, 3, in);
}

static void run_decroissante(void)
{
    static const int in[] = { 3, 2, 1, 5 };
    static const int expected[] = { 1, 2, 3, 5 };
    check_run(in, 4, 0, 4, 3, expected);
}

static void run_egaux_croissante(void)
{
    static const int in[] = { 2, 2, 2, 1 };
    check_run(in, 4, 0, 4, 3, in);
}

/* Une suite strictement décroissante s'arrête sur deux valeurs égales (stabilité). */
static void run_egaux_decroissante(void)
{
    static const int in[] = { 3, 2, 2, 1 };
    static const int expected[] = { 2, 3, 2, 1 };
    check_run(in, 4, 0, 4, 2, expected);
}

static void run_un(void)
{
    static const int in[] = { 5 };
    check_run(in, 1, 0, 1, 1, in);
}

static void run_deux(void)
{
    static const int in[] = { 2, 1 };
    static const int expected[] = { 1, 2 };
    check_run(in, 2, 0, 2, 2, expected);
}

static void run_zone(void)
{
    static const int in[] = { 9, 9, 4, 3, 8 };
    static const int expected[] = { 9, 9, 3, 4, 8 };
    check_run(in, 5, 2, 5, 2, expected);
}

static void run_borne_haute(void)
{
    static const int in[] = { 4, 3, 2, 1 };
    static const int expected[] = { 2, 3, 4, 1 };
    check_run(in, 4, 0, 3, 3, expected);
}

/* ---------------- Étape 2 · compute_minrun ---------------- */

static void check_minrun(size_t n, size_t expected)
{
    size_t got = compute_minrun(n);
    if (got == expected) {
        return;
    }
    tl_line("n = %zu", n);
    tl_line("attendu : %zu", expected);
    tl_line("obtenu  : %zu", got);
    if (n < 64) {
        tl_symptom("pour n < 64, minrun vaut n lui-même.");
        tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : que renvoies-tu "
                    "quand n est déjà sous 64 ?");
    } else if (got + 1 == expected) {
        tl_symptom("il manque le + 1 : un des bits retirés valait 1.");
        tl_question("Gardes-tu la trace des bits qui sortent à chaque division par 2 ?");
    } else if (got == expected + 1) {
        tl_symptom("un + 1 de trop : aucun bit retiré ne valait 1.");
        tl_question("Comment testes-tu si le bit qui sort vaut 1 ?");
    } else {
        tl_symptom("le résultat devrait être entre 32 et 64.");
        tl_question("Divises-tu n par 2 jusqu'à passer sous 64, ni plus, ni moins ?");
    }
    tl_fail();
}

static void minrun_petits(void)
{
    check_minrun(0, 0);
    check_minrun(1, 1);
    check_minrun(63, 63);
}

static void minrun_puissances(void)
{
    check_minrun(64, 32);
    check_minrun(128, 32);
    check_minrun((size_t)1 << 20, 32);
}

static void minrun_bits(void)
{
    check_minrun(65, 33);
    check_minrun(100, 50);
    check_minrun(127, 64);
    check_minrun(1000, 63);
    check_minrun(2112, 33);
}

/* ---------------- Étape 3 · merge_runs ---------------- */

static void check_merge(const int *in, size_t n, size_t lo, size_t mid, size_t hi,
                        const int *expected)
{
    int *a = tl_ints_copy(in, n);
    size_t tmp_n = mid - lo > 0 ? mid - lo : 1;
    int *tmp = tl_ints_new(tmp_n);
    tl_on_crash("tmp n'a que mid - lo cases : à quel indice y ranges-tu a[lo] ? Quand une "
                "suite est épuisée, lis-tu encore une de ses cases ?");
    merge_runs(a, tmp, lo, mid, hi);
    tl_check_guards(a, n, "merge_runs");
    tl_check_guards(tmp, tmp_n, "merge_runs (dans tmp, qui n'a que mid - lo cases)");
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        tl_ints_free(tmp);
        return;
    }
    tl_line("suites  : a[%zu..%zu) et a[%zu..%zu), tmp de %zu case(s)", lo, mid, mid, hi,
            mid - lo);
    tl_report_ints(in, expected, a, n, NULL, NULL);
    tl_perm p = tl_check_permutation(in, a, n);
    if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 3, c'est normal. Sinon : copies-tu la "
                    "suite de gauche dans tmp avant de fusionner dans a ?");
    } else if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Quand une suite est épuisée, que deviennent les éléments de l'autre ? "
                    "Écrases-tu une valeur de droite avant de l'avoir lue ?");
    } else {
        tl_symptom_diff(in, expected, a, n);
        tl_question("À chaque tour, compares-tu les deux têtes et prends-tu la plus petite "
                    "(celle de gauche en cas d'égalité) ?");
    }
    tl_fail();
}

static void fus_exemple(void)
{
    static const int in[] = { 1, 4, 7, 2, 3, 9 };
    static const int expected[] = { 1, 2, 3, 4, 7, 9 };
    check_merge(in, 6, 0, 3, 6, expected);
}

static void fus_gauche_epuisee(void)
{
    static const int in[] = { 1, 2, 3, 4, 5, 6 };
    check_merge(in, 6, 0, 3, 6, in);
}

static void fus_droite_epuisee(void)
{
    static const int in[] = { 4, 5, 6, 1, 2, 3 };
    static const int expected[] = { 1, 2, 3, 4, 5, 6 };
    check_merge(in, 6, 0, 3, 6, expected);
}

static void fus_zone(void)
{
    static const int in[] = { 9, 3, 8, 1, 5, 0 };
    static const int expected[] = { 9, 1, 3, 5, 8, 0 };
    check_merge(in, 6, 1, 3, 5, expected);
}

static void fus_tailles(void)
{
    static const int in[] = { 2, 9, 1, 3, 4, 5, 6, 7 };
    static const int expected[] = { 1, 2, 3, 4, 5, 6, 7, 9 };
    check_merge(in, 8, 0, 2, 8, expected);
}

static void fus_doublons(void)
{
    static const int in[] = { 1, 3, 3, 3, 4 };
    check_merge(in, 5, 0, 3, 5, in);
}

/* ---------------- Étape 4 · timsort ---------------- */

TL_SORT_SUITE_DEFINE(timsort, 200000)

static void sort_motif(int kind)
{
    enum { N = 1000000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1818);
    for (size_t i = 0; i < N; i++) {
        switch (kind) {
        case 0: /* presque trié */
            in[i] = (int)i;
            break;
        case 1: /* trié à l'envers */
            in[i] = (int)(N - i);
            break;
        case 2: /* 100 blocs triés mis bout à bout */
            in[i] = (int)((i % 10000) * 7 + i / 10000);
            break;
        default: /* aléatoire */
            in[i] = tl_rand_range(INT_MIN, INT_MAX);
            break;
        }
    }
    if (kind == 0) {
        for (size_t i = 0; i + 1 < N; i += 997) {
            int t = in[i];
            in[i] = in[i + 1];
            in[i + 1] = t;
        }
    }
    tl_on_crash("Ta pile de suites déborde-t-elle ? Fusionnes-tu dès qu'une des deux règles "
                "n'est plus respectée au sommet ?");
    tl_on_timeout("Profites-tu des suites déjà triées ? Les fusions respectent-elles les "
                  "règles de la pile ?");
    tl_check_sort_int("timsort", timsort, in, N, NULL);
    tl_ints_free(in);
}

static void sort_presque_trie(void)
{
    sort_motif(0);
}

static void sort_inverse_grand(void)
{
    sort_motif(1);
}

static void sort_blocs(void)
{
    sort_motif(2);
}

static void sort_aleatoire_grand(void)
{
    sort_motif(3);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "count_run", false },
    { 2, "compute_minrun", false },
    { 3, "merge_runs", false },
    { 4, "timsort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, run_croissante, "{1, 2, 3, 0} : suite croissante"),
    TL_TEST(1, run_decroissante, "{3, 2, 1, 5} : suite décroissante"),
    TL_TEST(1, run_egaux_croissante, "{2, 2, 2, 1} : égalités dans une croissante"),
    TL_TEST(1, run_egaux_decroissante, "{3, 2, 2, 1} : égalités dans une décroissante"),
    TL_TEST(1, run_un, "un seul élément"),
    TL_TEST(1, run_deux, "{2, 1}"),
    TL_TEST(1, run_zone, "zone a[2..5)"),
    TL_TEST(1, run_borne_haute, "la suite s'arrête à hi"),

    TL_TEST(2, minrun_petits, "n < 64"),
    TL_TEST(2, minrun_puissances, "puissances de 2"),
    TL_TEST(2, minrun_bits, "bits retirés à 1"),

    TL_TEST(3, fus_exemple, "fusion de {1, 4, 7} et {2, 3, 9}"),
    TL_TEST(3, fus_gauche_epuisee, "la suite de gauche s'épuise d'abord"),
    TL_TEST(3, fus_droite_epuisee, "la suite de droite s'épuise d'abord"),
    TL_TEST(3, fus_zone, "fusion dans la zone a[1..5)"),
    TL_TEST(3, fus_tailles, "suites de tailles 2 et 6"),
    TL_TEST(3, fus_doublons, "doublons"),

    TL_SORT_SUITE_ENTRIES(4, timsort),
    TL_TEST(4, sort_presque_trie, "1 000 000 éléments presque triés"),
    TL_TEST(4, sort_inverse_grand, "1 000 000 éléments triés à l'envers"),
    TL_TEST(4, sort_blocs, "100 blocs triés mis bout à bout"),
    TL_TEST(4, sort_aleatoire_grand, "1 000 000 éléments aléatoires"),
};

TL_MAIN("18", "Timsort simplifié", STEPS, TESTS)

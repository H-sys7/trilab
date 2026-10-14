/* Exercice 04 — tests. NE PAS MODIFIER CE FICHIER. */
#include "cocktail.h"
#include "tl_check.h"

typedef size_t (*range_pass_fn)(int *, size_t, size_t);

/* Erreur fréquente reconnue : résultat obtenu, symptôme et question associés. */
typedef struct {
    const int *got;
    size_t ret;
    const char *symptom;
    const char *question;
} known_bug;

static const known_bug NO_KNOWN_BUG = { NULL, 0, NULL, NULL };

/* Vérifie une passe sur a[lo..hi) : tableau attendu écrit en dur, nombre d'échanges. */
static void check_range_pass(const char *func, range_pass_fn pass, const int *in, size_t n,
                             size_t lo, size_t hi, const int *expected, size_t expected_ret,
                             known_bug known)
{
    bool forward = pass == cocktail_forward;
    int *a = tl_ints_copy(in, n);
    if (forward) {
        tl_on_crash("Ta comparaison lit-elle une case en dehors de a[lo..hi) ?");
    } else {
        tl_on_crash("Que devient ton indice quand il doit descendre jusqu'à lo ? Et si lo "
                    "vaut 0 ?");
        tl_on_timeout("Ta boucle descend-elle jusqu'à lo ? Que devient un size_t qui "
                      "descend sous 0 ?");
    }
    size_t ret = pass(a, lo, hi);
    tl_check_guards(a, n, func);
    if (ret == expected_ret && tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }

    char note_exp[64];
    char note_got[64];
    snprintf(note_exp, sizeof note_exp, "(échanges : %zu)", expected_ret);
    snprintf(note_got, sizeof note_got, "(échanges : %zu)", ret);
    tl_line("zone    : a[%zu..%zu)", lo, hi);
    tl_report_ints(in, expected, a, n, note_exp, note_got);

    if (ret == 0 && tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé et la fonction renvoie 0.");
        tl_question("Si tu n'as pas encore commencé cette étape, c'est normal. Sinon : "
                    "combien de tours fait ta boucle sur cette zone ?");
        tl_fail();
    }
    for (size_t i = 0; i < n; i++) {
        if ((i < lo || i >= hi) && a[i] != in[i]) {
            tl_symptom("la case a[%zu] est hors de la zone a[%zu..%zu), et elle a changé.",
                       i, lo, hi);
            tl_question("Quelles cases ta boucle a-t-elle le droit de toucher ?");
            tl_fail();
        }
    }
    if (known.got != NULL && known.ret == ret && tl_ints_equal(a, known.got, n)) {
        tl_symptom("%s", known.symptom);
        tl_question("%s", known.question);
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
    if (forward && a[hi - 1] != expected[hi - 1]) {
        tl_symptom("le plus grand élément de la zone (%d) n'est pas arrivé en a[%zu].",
                   expected[hi - 1], hi - 1);
        tl_question("Ta boucle examine-t-elle toutes les paires de la zone, jusqu'à la "
                    "dernière ?");
        tl_fail();
    }
    if (!forward && a[lo] != expected[lo]) {
        tl_symptom("le plus petit élément de la zone (%d) n'est pas arrivé en a[%zu].",
                   expected[lo], lo);
        tl_question("Ta boucle examine-t-elle toutes les paires de la zone, jusqu'à la "
                    "toute première ?");
        tl_fail();
    }
    tl_symptom_diff(in, expected, a, n);
    tl_question("Déroule ta passe à la main sur l'entrée : à quoi ressemble le tableau "
                "après chaque paire examinée ?");
    tl_fail();
}

/* ---------------- Étape 1 · cocktail_forward ---------------- */

static void fwd_exemple(void)
{
    static const int in[] = { 3, 1, 2, 9, 4 };
    static const int expected[] = { 1, 2, 3, 4, 9 };
    check_range_pass("cocktail_forward", cocktail_forward, in, 5, 0, 5, expected, 3,
                     NO_KNOWN_BUG);
}

static void fwd_zone(void)
{
    static const int in[] = { 9, 5, 1, 4, 0 };
    static const int expected[] = { 9, 1, 4, 5, 0 };
    check_range_pass("cocktail_forward", cocktail_forward, in, 5, 1, 4, expected, 2,
                     NO_KNOWN_BUG);
}

static void fwd_fin_zone(void)
{
    static const int in[] = { 1, 2, 9, 3, 0 };
    static const int expected[] = { 1, 2, 3, 9, 0 };
    check_range_pass("cocktail_forward", cocktail_forward, in, 5, 0, 4, expected, 1,
                     NO_KNOWN_BUG);
}

static void fwd_une_case(void)
{
    static const int in[] = { 4, 3, 2, 1 };
    check_range_pass("cocktail_forward", cocktail_forward, in, 4, 2, 3, in, 0, NO_KNOWN_BUG);
}

static void fwd_egaux(void)
{
    static const int in[] = { 2, 2, 1 };
    static const int expected[] = { 2, 1, 2 };
    known_bug known = { expected, 2, "le tableau est correct, mais 2 échanges sont comptés "
                        "au lieu de 1 : les deux 2 voisins ont été échangés entre eux.",
                        "Deux voisins égaux sont-ils mal ordonnés ?" };
    check_range_pass("cocktail_forward", cocktail_forward, in, 3, 0, 3, expected, 1, known);
}

static void fwd_extremes(void)
{
    static const int in[] = { INT_MAX, INT_MIN, 0 };
    static const int expected[] = { INT_MIN, 0, INT_MAX };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_range_pass("cocktail_forward", cocktail_forward, in, 3, 0, 3, expected, 2,
                     NO_KNOWN_BUG);
}

/* ---------------- Étape 2 · cocktail_backward ---------------- */

static void bwd_exemple(void)
{
    static const int in[] = { 3, 1, 2, 9, 4 };
    static const int expected[] = { 1, 3, 2, 4, 9 };
    static const int forward_result[] = { 1, 2, 3, 4, 9 };
    known_bug known = { forward_result, 3, "c'est le résultat d'une passe de gauche à "
                        "droite.", "Dans quel sens ta boucle parcourt-elle la zone ?" };
    check_range_pass("cocktail_backward", cocktail_backward, in, 5, 0, 5, expected, 2, known);
}

static void bwd_inverse(void)
{
    static const int in[] = { 5, 4, 3, 2, 1 };
    static const int expected[] = { 1, 5, 4, 3, 2 };
    check_range_pass("cocktail_backward", cocktail_backward, in, 5, 0, 5, expected, 4,
                     NO_KNOWN_BUG);
}

static void bwd_zone(void)
{
    static const int in[] = { 9, 7, 8, 3, -1 };
    static const int expected[] = { 9, 3, 7, 8, -1 };
    check_range_pass("cocktail_backward", cocktail_backward, in, 5, 1, 4, expected, 2,
                     NO_KNOWN_BUG);
}

static void bwd_une_case(void)
{
    static const int in[] = { 4, 3, 2, 1 };
    check_range_pass("cocktail_backward", cocktail_backward, in, 4, 0, 1, in, 0,
                     NO_KNOWN_BUG);
}

static void bwd_egaux(void)
{
    static const int in[] = { 2, 1, 1 };
    static const int expected[] = { 1, 2, 1 };
    known_bug known = { expected, 2, "le tableau est correct, mais 2 échanges sont comptés "
                        "au lieu de 1 : les deux 1 voisins ont été échangés entre eux.",
                        "Deux voisins égaux sont-ils mal ordonnés ?" };
    check_range_pass("cocktail_backward", cocktail_backward, in, 3, 0, 3, expected, 1, known);
}

static void bwd_extremes(void)
{
    static const int in[] = { 0, INT_MAX, INT_MIN };
    static const int expected[] = { INT_MIN, 0, INT_MAX };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_range_pass("cocktail_backward", cocktail_backward, in, 3, 0, 3, expected, 2,
                     NO_KNOWN_BUG);
}

/* ---------------- Étape 3 · cocktail_sort ---------------- */

TL_SORT_SUITE_DEFINE(cocktail_sort, 5000)

/* Une grande valeur au début : un aller suffit, puis un retour sans échange. */
static void sort_arret_anticipe(void)
{
    enum { N = 300000 };
    int *in = tl_ints_new(N);
    in[0] = N;
    for (size_t i = 1; i < N; i++) {
        in[i] = (int)i;
    }
    tl_on_timeout("Quand une passe ne fait aucun échange, que sais-tu du tableau ? "
                  "Ton tri s'en sert-il pour s'arrêter ?");
    tl_check_sort_int("cocktail_sort", cocktail_sort, in, N, NULL);
    tl_ints_free(in);
}

/* Une petite valeur à la fin (une « tortue ») : un seul retour doit la ramener en tête. */
static void sort_tortue(void)
{
    enum { N = 300000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i + 1 < N; i++) {
        in[i] = (int)i;
    }
    in[N - 1] = -1;
    tl_on_timeout("La petite valeur en fin de tableau recule-t-elle d'une seule case par "
                  "tour, ou jusqu'au début en un seul retour ?");
    tl_check_sort_int("cocktail_sort", cocktail_sort, in, N, NULL);
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "cocktail_forward", false },
    { 2, "cocktail_backward", false },
    { 3, "cocktail_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, fwd_exemple, "aller sur {3, 1, 2, 9, 4}"),
    TL_TEST(1, fwd_zone, "aller sur la zone a[1..4) de {9, 5, 1, 4, 0}"),
    TL_TEST(1, fwd_fin_zone, "aller sur a[0..4) de {1, 2, 9, 3, 0}"),
    TL_TEST(1, fwd_une_case, "aller sur une zone d'une case"),
    TL_TEST(1, fwd_egaux, "aller sur {2, 2, 1}"),
    TL_TEST(1, fwd_extremes, "aller sur {INT_MAX, INT_MIN, 0}"),

    TL_TEST(2, bwd_exemple, "retour sur {3, 1, 2, 9, 4}"),
    TL_TEST(2, bwd_inverse, "retour sur {5, 4, 3, 2, 1}"),
    TL_TEST(2, bwd_zone, "retour sur la zone a[1..4) de {9, 7, 8, 3, -1}"),
    TL_TEST(2, bwd_une_case, "retour sur la zone a[0..1)"),
    TL_TEST(2, bwd_egaux, "retour sur {2, 1, 1}"),
    TL_TEST(2, bwd_extremes, "retour sur {0, INT_MAX, INT_MIN}"),

    TL_SORT_SUITE_ENTRIES(3, cocktail_sort),
    TL_TEST_T(3, sort_arret_anticipe, "300 000 éléments, grande valeur au début", 3),
    TL_TEST_T(3, sort_tortue, "300 000 éléments, petite valeur à la fin", 3),
};

TL_MAIN("04", "Tri cocktail", STEPS, TESTS)

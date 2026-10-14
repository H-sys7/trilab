/* Exercice 01 — tests. NE PAS MODIFIER CE FICHIER. */
#include "bubble.h"
#include "tl_check.h"

typedef size_t (*pass_fn)(int *, size_t);

/* Erreur fréquente reconnue : résultat obtenu, symptôme et question associés. */
typedef struct {
    const int *got;
    size_t ret;
    const char *symptom;
    const char *question;
} known_bug;

static const known_bug NO_KNOWN_BUG = { NULL, 0, NULL, NULL };

/* Vérifie une passe : tableau attendu (écrit en dur) et valeur renvoyée.
 * what : « échanges » pour bubble_pass, « position » pour bubble_pass_last. */
static void check_pass(const char *func, pass_fn pass, const char *what, const int *in,
                       const int *expected, size_t n, size_t expected_ret, known_bug known)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta boucle tourne-t-elle quand même ? Que vaut n - 1 quand "
                    "n est un size_t égal à 0 ?");
        size_t ret = pass(NULL, 0);
        if (ret != expected_ret) {
            tl_line("entrée  : a = NULL, n = 0");
            tl_line("attendu : %zu", expected_ret);
            tl_line("obtenu  : %zu", ret);
            tl_symptom("un tableau vide ne demande aucun échange.");
            tl_question("Que compte ta fonction quand il n'y a aucune paire de voisins ?");
            tl_fail();
        }
        return;
    }
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta comparaison lit-elle une case en dehors de a[0..n) ? Quel est le "
                "dernier indice valide ?");
    size_t ret = pass(a, n);
    tl_check_guards(a, n, func);
    if (ret == expected_ret && tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }

    char note_exp[64];
    char note_got[64];
    snprintf(note_exp, sizeof note_exp, "(%s : %zu)", what, expected_ret);
    snprintf(note_got, sizeof note_got, "(%s : %zu)", what, ret);
    tl_report_ints(in, expected, a, n, note_exp, note_got);

    if (ret == 0 && tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé et la fonction renvoie 0.");
        tl_question("Si tu n'as pas encore commencé cette étape, c'est normal : ouvre "
                    "bubble.c au TODO correspondant. Sinon : pour %zu éléments, combien "
                    "de tours fait ta boucle ?", n);
        tl_fail();
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
        if (pass == bubble_pass) {
            tl_question("À quel moment ton compteur augmente-t-il : à chaque comparaison, "
                        "ou seulement quand un échange a lieu ?");
        } else {
            tl_question("Lors du dernier échange de la passe, dans quelle case arrive le "
                        "plus grand des deux éléments ?");
        }
        tl_fail();
    }
    if (a[n - 1] != expected[n - 1]) {
        tl_symptom("après la passe, le plus grand élément (%d) n'est pas en dernière "
                   "position : a[%zu] = %d.", expected[n - 1], n - 1, a[n - 1]);
        tl_question("Ta boucle examine-t-elle toutes les paires de voisins, jusqu'à la "
                    "toute dernière ?");
        tl_fail();
    }
    if (tl_first_unsorted(a, n) == n && tl_first_unsorted(expected, n) != n) {
        tl_symptom("le tableau est entièrement trié : ta fonction en fait plus qu'une "
                   "seule passe.");
        tl_question("Combien de fois une passe parcourt-elle le tableau ?");
        tl_fail();
    }
    tl_symptom_diff(in, expected, a, n);
    tl_question("Déroule une passe à la main sur l'entrée : à quoi ressemble le tableau "
                "après chaque paire examinée ?");
    tl_fail();
}

/* ---------------- Étape 1 · bubble_pass ---------------- */

static void pass_vide(void)
{
    check_pass("bubble_pass", bubble_pass, "échanges", NULL, NULL, 0, 0, NO_KNOWN_BUG);
}

static void pass_un(void)
{
    static const int in[] = { 7 };
    check_pass("bubble_pass", bubble_pass, "échanges", in, in, 1, 0, NO_KNOWN_BUG);
}

static void pass_deux(void)
{
    static const int in[] = { 2, 1 };
    static const int expected[] = { 1, 2 };
    check_pass("bubble_pass", bubble_pass, "échanges", in, expected, 2, 1, NO_KNOWN_BUG);
}

static void pass_exemple(void)
{
    static const int in[] = { 3, 1, 2, 9, 4 };
    static const int expected[] = { 1, 2, 3, 4, 9 };
    static const int short_loop[] = { 1, 2, 3, 9, 4 };
    known_bug known = { short_loop, 2, "la dernière paire de voisins (9, 4) n'a pas été "
                        "traitée.", "Pour 5 éléments, quel est l'indice du dernier élément "
                        "que ta boucle atteint ?" };
    check_pass("bubble_pass", bubble_pass, "échanges", in, expected, 5, 3, known);
}

static void pass_deja_trie(void)
{
    static const int in[] = { 1, 2, 3, 4, 5 };
    check_pass("bubble_pass", bubble_pass, "échanges", in, in, 5, 0, NO_KNOWN_BUG);
}

static void pass_inverse(void)
{
    static const int in[] = { 5, 4, 3, 2, 1 };
    static const int expected[] = { 4, 3, 2, 1, 5 };
    check_pass("bubble_pass", bubble_pass, "échanges", in, expected, 5, 4, NO_KNOWN_BUG);
}

static void pass_egaux(void)
{
    static const int in[] = { 2, 2, 1 };
    static const int expected[] = { 2, 1, 2 };
    known_bug known = { expected, 2, "le tableau est correct, mais 2 échanges sont comptés "
                        "au lieu de 1 : les deux 2 voisins ont été échangés entre eux.",
                        "Deux voisins égaux sont-ils mal ordonnés ?" };
    check_pass("bubble_pass", bubble_pass, "échanges", in, expected, 3, 1, known);
}

static void pass_extremes(void)
{
    static const int in[] = { INT_MAX, INT_MIN, 0, -5 };
    static const int expected[] = { INT_MIN, 0, -5, INT_MAX };
    static const int no_first_swap[] = { INT_MAX, INT_MIN, -5, 0 };
    known_bug known = { no_first_swap, 1, "INT_MAX et INT_MIN n'ont pas été échangés, "
                        "alors que INT_MAX est plus grand.",
                        "Ta comparaison fait-elle un calcul ? Que vaut INT_MAX - INT_MIN "
                        "dans un int ?" };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_pass("bubble_pass", bubble_pass, "échanges", in, expected, 4, 3, known);
}

/* ---------------- Étape 2 · bubble_sort ---------------- */

TL_SORT_SUITE_DEFINE(bubble_sort, 5000)

/* Une passe suffit à ranger le tableau : sans arrêt anticipé, le tri dépasse le temps. */
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
    tl_check_sort_int("bubble_sort", bubble_sort, in, N, NULL);
    tl_ints_free(in);
}

/* ---------------- Bonus 1 · bubble_pass_last ---------------- */

static void last_vide(void)
{
    check_pass("bubble_pass_last", bubble_pass_last, "position", NULL, NULL, 0, 0,
               NO_KNOWN_BUG);
}

static void last_exemple(void)
{
    static const int in[] = { 3, 1, 2, 9, 4 };
    static const int expected[] = { 1, 2, 3, 4, 9 };
    check_pass("bubble_pass_last", bubble_pass_last, "position", in, expected, 5, 4,
               NO_KNOWN_BUG);
}

static void last_au_debut(void)
{
    static const int in[] = { 2, 1, 3, 4, 5 };
    static const int expected[] = { 1, 2, 3, 4, 5 };
    check_pass("bubble_pass_last", bubble_pass_last, "position", in, expected, 5, 1,
               NO_KNOWN_BUG);
}

static void last_au_milieu(void)
{
    static const int in[] = { 1, 3, 2, 4, 5 };
    static const int expected[] = { 1, 2, 3, 4, 5 };
    check_pass("bubble_pass_last", bubble_pass_last, "position", in, expected, 5, 2,
               NO_KNOWN_BUG);
}

static void last_aucun(void)
{
    static const int in[] = { 1, 2, 3 };
    check_pass("bubble_pass_last", bubble_pass_last, "position", in, in, 3, 0,
               NO_KNOWN_BUG);
}

static void last_inverse(void)
{
    static const int in[] = { 5, 4, 3, 2, 1 };
    static const int expected[] = { 4, 3, 2, 1, 5 };
    check_pass("bubble_pass_last", bubble_pass_last, "position", in, expected, 5, 4,
               NO_KNOWN_BUG);
}

/* ---------------- Bonus 2 · bubble_sort_last ---------------- */

TL_SORT_SUITE_DEFINE(bubble_sort_last, 5000)

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "bubble_pass", false },
    { 2, "bubble_sort", false },
    { 3, "bubble_pass_last", true },
    { 4, "bubble_sort_last", true },
};

static const tl_test TESTS[] = {
    TL_TEST(1, pass_vide, "une passe sur n = 0 avec a = NULL"),
    TL_TEST(1, pass_un, "une passe sur {7}"),
    TL_TEST(1, pass_deux, "une passe sur {2, 1}"),
    TL_TEST(1, pass_exemple, "une passe sur {3, 1, 2, 9, 4}"),
    TL_TEST(1, pass_deja_trie, "une passe sur {1, 2, 3, 4, 5}"),
    TL_TEST(1, pass_inverse, "une passe sur {5, 4, 3, 2, 1}"),
    TL_TEST(1, pass_egaux, "une passe sur {2, 2, 1}"),
    TL_TEST(1, pass_extremes, "une passe sur {INT_MAX, INT_MIN, 0, -5}"),

    TL_SORT_SUITE_ENTRIES(2, bubble_sort),
    TL_TEST_T(2, sort_arret_anticipe, "300 000 éléments rangés par une seule passe", 3),

    TL_TEST(3, last_vide, "n = 0 avec a = NULL"),
    TL_TEST(3, last_exemple, "dernier échange en fin de tableau"),
    TL_TEST(3, last_au_debut, "dernier échange au début"),
    TL_TEST(3, last_au_milieu, "dernier échange au milieu"),
    TL_TEST(3, last_aucun, "aucun échange"),
    TL_TEST(3, last_inverse, "trié à l'envers"),

    TL_SORT_SUITE_ENTRIES(4, bubble_sort_last),
};

TL_MAIN("01", "Tri à bulles", STEPS, TESTS)

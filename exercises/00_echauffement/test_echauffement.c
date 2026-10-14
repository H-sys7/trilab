/* Exercice 00 — tests. NE PAS MODIFIER CE FICHIER. */
#include "echauffement.h"
#include "tl_check.h"

/* ---------------- Étape 1 · swap ---------------- */

static void check_swap(int x0, int y0)
{
    int x = x0;
    int y = y0;
    swap(&x, &y);
    if (x == y0 && y == x0) {
        return;
    }
    tl_line("avant   : x = %d, y = %d", x0, y0);
    tl_line("attendu : x = %d, y = %d", y0, x0);
    tl_line("obtenu  : x = %d, y = %d", x, y);
    if (x == x0 && y == y0) {
        tl_symptom("x et y n'ont pas changé.");
        tl_question("Ta fonction modifie-t-elle les cases pointées par x et y, ou seulement "
                    "des copies locales ? Si tu n'as pas commencé l'étape 1, c'est normal.");
    } else if (x == y) {
        tl_symptom("x et y valent tous les deux %d : la valeur %d a été perdue.", x,
                   x == x0 ? y0 : x0);
        tl_question("Que contient *x juste après ta première affectation ?");
    } else {
        tl_symptom("les valeurs obtenues ne sont pas celles de départ.");
        tl_question("Ta méthode fait-elle un calcul sur les valeurs ? Que se passe-t-il "
                    "s'il dépasse les limites d'un int ?");
    }
    tl_fail();
}

static void swap_simple(void)
{
    check_swap(3, 7);
}

static void swap_negatifs(void)
{
    check_swap(-4, 9);
}

static void swap_extremes(void)
{
    tl_on_ub("Ta méthode fait-elle un calcul qui peut dépasser INT_MAX ou descendre "
             "sous INT_MIN ?");
    check_swap(INT_MAX, INT_MIN);
}

static void swap_grands(void)
{
    tl_on_ub("Ta méthode fait-elle un calcul qui peut dépasser INT_MAX ou descendre "
             "sous INT_MIN ?");
    check_swap(INT_MAX, INT_MAX - 1);
}

static void swap_meme_case(void)
{
    int x = 5;
    swap(&x, &x);
    if (x == 5) {
        return;
    }
    tl_line("avant   : x = 5, puis swap(&x, &x)");
    tl_line("attendu : x = 5");
    tl_line("obtenu  : x = %d", x);
    tl_symptom("x et y désignaient la même case, et sa valeur a changé.");
    tl_question("Ta méthode marche-t-elle encore quand x et y sont la même adresse ?");
    tl_fail();
}

static void swap_dans_tableau(void)
{
    static const int in[] = { 10, 20, 30, 40, 50 };
    static const int expected[] = { 10, 40, 30, 20, 50 };
    int *a = tl_ints_copy(in, TL_LEN(in));
    swap(&a[1], &a[3]);
    tl_check_guards(a, TL_LEN(in), "swap");
    if (tl_ints_equal(a, expected, TL_LEN(in))) {
        tl_ints_free(a);
        return;
    }
    tl_line("appel   : swap(&a[1], &a[3])");
    tl_report_ints(in, expected, a, TL_LEN(in), NULL, NULL);
    tl_symptom_diff(in, expected, a, TL_LEN(in));
    tl_question("&a[1] est l'adresse d'une case du tableau : ta fonction écrit-elle "
                "bien dans les cases désignées, et seulement dans celles-là ?");
    tl_fail();
}

/* ---------------- Étape 2 · min_index ---------------- */

static void check_min(const int *in, size_t n, size_t expected)
{
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle lit-elle une case en dehors de a[0..n) ?");
    size_t got = min_index(a, n);
    tl_check_guards(a, n, "min_index");
    if (!tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau a été modifié, alors que min_index doit seulement le lire.");
        tl_question("Quelle instruction de ta fonction écrit dans le tableau ?");
        tl_fail();
    }
    if (got == expected) {
        tl_ints_free(a);
        return;
    }
    tl_show("entrée", in, n, got < n ? got : expected, NULL);
    tl_line("attendu : indice %zu (valeur %d)", expected, in[expected]);
    if (got >= n) {
        tl_line("obtenu  : indice %zu", got);
        tl_symptom("l'indice renvoyé est en dehors du tableau.");
        tl_question("Pour %zu éléments, quels sont les indices valides ?", n);
    } else if (in[got] == in[expected]) {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("bonne valeur, mais ce n'est pas sa première apparition.");
        tl_question("Quand tu croises une valeur égale au plus petit trouvé jusqu'ici, "
                    "lequel des deux indices gardes-tu ?");
    } else if (expected == n - 1) {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("le minimum est dans la dernière case, et il n'a pas été retenu.");
        tl_question("Pour %zu éléments, ta boucle examine-t-elle la toute dernière case ?",
                    n);
    } else if (got == 0 && n > 1) {
        tl_line("obtenu  : indice 0 (valeur %d)", in[0]);
        tl_symptom("la fonction renvoie 0, la valeur de départ.");
        tl_question("Ta fonction parcourt-elle le tableau ? Si tu n'as pas commencé "
                    "l'étape 2, c'est normal.");
    } else {
        tl_line("obtenu  : indice %zu (valeur %d)", got, in[got]);
        tl_symptom("%d n'est pas la plus petite valeur : %d est plus petit.", in[got],
                   in[expected]);
        tl_question("Déroule ta fonction à la main sur cette entrée : quel indice "
                    "retiens-tu après chaque case ?");
    }
    tl_fail();
}

static void min_un_element(void)
{
    static const int in[] = { 8 };
    check_min(in, TL_LEN(in), 0);
}

static void min_au_debut(void)
{
    static const int in[] = { 1, 5, 3 };
    check_min(in, TL_LEN(in), 0);
}

static void min_au_milieu(void)
{
    static const int in[] = { 4, -2, 6 };
    check_min(in, TL_LEN(in), 1);
}

static void min_a_la_fin(void)
{
    static const int in[] = { 5, 4, 3, 2, 1 };
    check_min(in, TL_LEN(in), 4);
}

static void min_egalites(void)
{
    static const int in[] = { 4, 1, 3, 1 };
    check_min(in, TL_LEN(in), 1);
}

static void min_extremes(void)
{
    static const int in[] = { 0, INT_MAX, INT_MIN, -1, INT_MIN };
    check_min(in, TL_LEN(in), 2);
}

static void min_aleatoire(void)
{
    enum { N = 1000, POS = 637 };
    int *in = tl_ints_new(N);
    tl_rand_seed(3003);
    tl_fill_random(in, N, 0, 1000000);
    in[POS] = -1;
    check_min(in, N, POS);
    tl_ints_free(in);
}

/* ---------------- Étape 3 · reverse ---------------- */

static void check_reverse(const int *in, const int *expected, size_t n)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta fonction lit-elle ou écrit-elle quand même une case ? "
                    "Que vaut n - 1 quand n est un size_t égal à 0 ?");
        reverse(NULL, 0);
        return;
    }
    int *a = tl_ints_copy(in, n);
    tl_on_crash("Ta boucle lit-elle ou écrit-elle une case en dehors de a[0..n) ?");
    reverse(a, n);
    tl_check_guards(a, n, "reverse");
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }
    tl_report_ints(in, expected, a, n, NULL, NULL);
    tl_perm p = tl_check_permutation(in, a, n);
    if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau est identique à l'entrée.");
        tl_question("Si tu n'as pas commencé l'étape 3, c'est normal. Sinon : que "
                    "devient une paire de cases échangée deux fois ?");
    } else if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Comment échanges-tu deux cases sans perdre l'une des deux valeurs ?");
    } else {
        size_t i = tl_first_diff(expected, a, n);
        tl_symptom("la case a[%zu] contient %d au lieu de %d.", i, a[i], expected[i]);
        tl_question("Quelle case doit être échangée avec a[%zu] ? Où s'arrête ta boucle ?",
                    i);
    }
    tl_fail();
}

static void rev_vide(void)
{
    check_reverse(NULL, NULL, 0);
}

static void rev_un(void)
{
    static const int in[] = { 1 };
    check_reverse(in, in, TL_LEN(in));
}

static void rev_deux(void)
{
    static const int in[] = { 1, 2 };
    static const int expected[] = { 2, 1 };
    check_reverse(in, expected, TL_LEN(in));
}

static void rev_impair(void)
{
    static const int in[] = { 1, 2, 3, 4, 5 };
    static const int expected[] = { 5, 4, 3, 2, 1 };
    check_reverse(in, expected, TL_LEN(in));
}

static void rev_pair(void)
{
    static const int in[] = { 1, 2, 3, 4, 5, 6 };
    static const int expected[] = { 6, 5, 4, 3, 2, 1 };
    check_reverse(in, expected, TL_LEN(in));
}

static void rev_mots(void)
{
    static const int in[] = { 8, -3, 8, 0, 42, 7, -3, 1, 9 };
    static const int expected[] = { 9, 1, -3, 7, 42, 0, 8, -3, 8 };
    check_reverse(in, expected, TL_LEN(in));
}

/* Grand tableau : propriétés seulement (extrémités, milieu, valeurs, double inversion). */
static void rev_aleatoire(void)
{
    enum { N = 1001 };
    int *in = tl_ints_new(N);
    tl_rand_seed(4004);
    tl_fill_random(in, N, -1000000, 1000000);
    int *a = tl_ints_copy(in, N);
    tl_on_crash("Ta boucle lit-elle ou écrit-elle une case en dehors de a[0..n) ?");
    reverse(a, N);
    tl_check_guards(a, N, "reverse");
    tl_perm p = tl_check_permutation(in, a, N);
    if (!p.ok) {
        tl_line("1001 éléments aléatoires");
        tl_symptom_permutation(p);
        tl_question("Comment échanges-tu deux cases sans perdre l'une des deux valeurs ?");
        tl_fail();
    }
    if (a[0] != in[N - 1] || a[N - 1] != in[0] || a[N / 2] != in[N / 2]) {
        tl_line("1001 éléments aléatoires");
        tl_line("a[0]    : attendu %d, obtenu %d", in[N - 1], a[0]);
        tl_line("a[500]  : attendu %d, obtenu %d", in[N / 2], a[N / 2]);
        tl_line("a[1000] : attendu %d, obtenu %d", in[0], a[N - 1]);
        tl_symptom("les extrémités ou le milieu ne sont pas à la bonne place.");
        tl_question("Tes tests sur de petits tableaux passent-ils ? Sinon, commence par "
                    "eux.");
        tl_fail();
    }
    reverse(a, N);
    if (!tl_ints_equal(a, in, N)) {
        size_t i = tl_first_diff(in, a, N);
        tl_line("1001 éléments aléatoires, inversés deux fois de suite");
        tl_show("attendu", in, N, i, NULL);
        tl_show("obtenu", a, N, i, NULL);
        tl_symptom("inverser deux fois ne redonne pas le tableau de départ.");
        tl_question("Quelle case n'a pas été échangée avec sa case symétrique ?");
        tl_fail();
    }
    tl_ints_free(a);
    tl_ints_free(in);
}

/* ---------------- Étape 4 · is_sorted ---------------- */

static void check_sorted(const int *in, size_t n, bool expected)
{
    const int *a = in;
    int *copy = NULL;
    if (n > 0) {
        copy = tl_ints_copy(in, n);
        a = copy;
    }
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta boucle tourne-t-elle quand même ? Que vaut n - 1 quand "
                    "n est un size_t égal à 0 ?");
    } else {
        tl_on_crash("Ta boucle lit-elle une case en dehors de a[0..n) ? Quel est le dernier "
                    "indice valide ?");
    }
    bool got = is_sorted(a, n);
    if (got == expected) {
        tl_ints_free(copy);
        return;
    }
    if (n > 0) {
        tl_show("entrée", in, n, expected ? 0 : tl_first_unsorted(in, n), NULL);
    } else {
        tl_line("entrée  : a = NULL, n = 0");
    }
    tl_line("attendu : %s", expected ? "true" : "false");
    tl_line("obtenu  : %s", got ? "true" : "false");
    if (expected) {
        tl_symptom("ce tableau est trié, mais is_sorted renvoie false.");
        tl_question("Quelle paire t'a fait conclure que le tableau n'est pas trié ? "
                    "Est-elle vraiment mal ordonnée, et ses deux cases sont-elles dans le "
                    "tableau ? Si tu n'as pas commencé l'étape 4, c'est normal.");
    } else {
        size_t i = tl_first_unsorted(in, n);
        tl_symptom("a[%zu] = %d est placé avant a[%zu] = %d, mais is_sorted renvoie true.",
                   i, in[i], i + 1, in[i + 1]);
        tl_question("Ta boucle examine-t-elle cette paire de voisins ? Ta comparaison "
                    "peut-elle dépasser les limites d'un int ?");
    }
    tl_fail();
}

static void sorted_vide(void)
{
    check_sorted(NULL, 0, true);
}

static void sorted_un(void)
{
    static const int in[] = { 7 };
    check_sorted(in, TL_LEN(in), true);
}

static void sorted_croissant(void)
{
    static const int in[] = { 1, 2, 3, 4 };
    check_sorted(in, TL_LEN(in), true);
}

static void sorted_egalites(void)
{
    static const int in[] = { 1, 2, 2, 3 };
    check_sorted(in, TL_LEN(in), true);
}

static void sorted_tous_egaux(void)
{
    static const int in[] = { 5, 5, 5 };
    check_sorted(in, TL_LEN(in), true);
}

static void sorted_extremes(void)
{
    static const int in[] = { INT_MIN, -1, 0, INT_MAX };
    check_sorted(in, TL_LEN(in), true);
}

static void unsorted_debut(void)
{
    static const int in[] = { 2, 1, 3, 4 };
    check_sorted(in, TL_LEN(in), false);
}

static void unsorted_milieu(void)
{
    static const int in[] = { 1, 3, 2, 4 };
    check_sorted(in, TL_LEN(in), false);
}

static void unsorted_fin(void)
{
    static const int in[] = { 1, 2, 3, 5, 4 };
    check_sorted(in, TL_LEN(in), false);
}

static void unsorted_extremes(void)
{
    static const int in[] = { INT_MAX, INT_MIN };
    check_sorted(in, TL_LEN(in), false);
}

static void sorted_grand(void)
{
    enum { N = 100000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = (int)i * 3 - 150000;
    }
    check_sorted(in, N, true);
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "swap", false },
    { 2, "min_index", false },
    { 3, "reverse", false },
    { 4, "is_sorted", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, swap_simple, "échanger 3 et 7"),
    TL_TEST(1, swap_negatifs, "échanger -4 et 9"),
    TL_TEST(1, swap_dans_tableau, "échanger deux cases d'un tableau"),
    TL_TEST(1, swap_extremes, "échanger INT_MAX et INT_MIN"),
    TL_TEST(1, swap_grands, "échanger INT_MAX et INT_MAX - 1"),
    TL_TEST(1, swap_meme_case, "échanger une case avec elle-même"),

    TL_TEST(2, min_un_element, "1 élément"),
    TL_TEST(2, min_au_debut, "minimum au début"),
    TL_TEST(2, min_au_milieu, "minimum au milieu"),
    TL_TEST(2, min_a_la_fin, "minimum dans la dernière case"),
    TL_TEST(2, min_egalites, "minimum présent deux fois"),
    TL_TEST(2, min_extremes, "INT_MIN et INT_MAX"),
    TL_TEST(2, min_aleatoire, "1000 éléments aléatoires"),

    TL_TEST(3, rev_vide, "n = 0 avec a = NULL"),
    TL_TEST(3, rev_un, "1 élément"),
    TL_TEST(3, rev_deux, "2 éléments"),
    TL_TEST(3, rev_impair, "nombre impair d'éléments"),
    TL_TEST(3, rev_pair, "nombre pair d'éléments"),
    TL_TEST(3, rev_mots, "valeurs répétées et négatives"),
    TL_TEST(3, rev_aleatoire, "1001 éléments aléatoires"),

    TL_TEST(4, sorted_vide, "n = 0 avec a = NULL : trié"),
    TL_TEST(4, sorted_un, "1 élément : trié"),
    TL_TEST(4, sorted_croissant, "croissant : trié"),
    TL_TEST(4, sorted_egalites, "avec des égalités : trié"),
    TL_TEST(4, sorted_tous_egaux, "tous égaux : trié"),
    TL_TEST(4, sorted_extremes, "INT_MIN à INT_MAX : trié"),
    TL_TEST(4, unsorted_debut, "désordre au début : non trié"),
    TL_TEST(4, unsorted_milieu, "désordre au milieu : non trié"),
    TL_TEST(4, unsorted_fin, "désordre sur la dernière paire : non trié"),
    TL_TEST(4, unsorted_extremes, "INT_MAX puis INT_MIN : non trié"),
    TL_TEST(4, sorted_grand, "100 000 éléments croissants : trié"),
};

TL_MAIN("00", "Échauffement : les pointeurs", STEPS, TESTS)

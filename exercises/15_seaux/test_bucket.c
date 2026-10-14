/* Exercice 15 — tests. NE PAS MODIFIER CE FICHIER. */
#include "bucket.h"
#include "tl_check.h"

/* ---------------- Outils pour les tableaux de double ---------------- */

#define DGUARD (-1.0) /* sentinelle autour des tableaux (mode base seulement) */

static double *doubles_new(size_t n)
{
    double *base = tl_malloc((n + 2 * TL_GUARDS) * sizeof(double));
#if TL_GUARDS
    base[0] = DGUARD;
    base[n + 1] = DGUARD;
#endif
    return base + TL_GUARDS;
}

static void doubles_free(double *a)
{
    if (a != NULL) {
        tl_free(a - TL_GUARDS);
    }
}

static int cmp_double(const void *x, const void *y)
{
    double a = *(const double *)x;
    double b = *(const double *)y;
    return (a > b) - (a < b);
}

static void show_doubles(const char *label, const double *a, size_t n, size_t focus)
{
    size_t from = 0;
    size_t to = n;
    if (n > TL_SHOW_MAX) {
        from = focus > TL_WINDOW ? focus - TL_WINDOW : 0;
        to = focus + TL_WINDOW + 1 < n ? focus + TL_WINDOW + 1 : n;
    }
    size_t w = tl_width(label, strlen(label));
    printf("    %s%*s: %s", label, (int)(w < 8 ? 8 - w : 0), "", from > 0 ? "{… " : "{");
    for (size_t i = from; i < to; i++) {
        printf(i > from ? ", %.6g" : "%.6g", a[i]);
    }
    printf("%s", to < n ? " …}" : "}");
    if (n > TL_SHOW_MAX) {
        printf(" (indices %zu à %zu sur %zu)", from, to - 1, n);
    }
    putchar('\n');
}

/* ---------------- Étape 1 · bucket_index ---------------- */

static void check_index(double x, size_t k, size_t expected)
{
    size_t got = bucket_index(x, k);
    if (got == expected) {
        return;
    }
    tl_line("entrée  : x = %.17g, k = %zu", x, k);
    tl_line("attendu : seau %zu", expected);
    tl_line("obtenu  : seau %zu", got);
    if (got >= k) {
        tl_symptom("le numéro de seau est >= k : ce seau n'existe pas.");
        tl_question("Que vaut la partie entière de x × k quand x est juste en dessous de 1 ? "
                    "Comment garantis-tu un résultat < k ?");
    } else if (got == 0) {
        tl_symptom("la fonction renvoie 0.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : quel est le "
                    "type de x × k, et comment en prends-tu la partie entière ?");
    } else if (got == expected + 1) {
        tl_symptom("le résultat est arrondi vers le haut.");
        tl_question("La conversion d'un double en entier garde-t-elle la partie entière, ou "
                    "arrondis-tu toi-même ?");
    } else {
        tl_symptom("ce n'est pas la partie entière de x × k.");
        tl_question("Calcule x × k à la main : quelle est sa partie entière ?");
    }
    tl_fail();
}

static void idx_zero(void)
{
    check_index(0.0, 10, 0);
}

static void idx_dixieme(void)
{
    check_index(0.05, 10, 0);
}

static void idx_moitie(void)
{
    check_index(0.5, 4, 2);
}

static void idx_quart(void)
{
    check_index(0.25, 4, 1);
}

static void idx_presque_un(void)
{
    check_index(0.99, 10, 9);
}

static void idx_juste_sous_un(void)
{
    check_index(0.9999999999999999, 7, 6);
}

static void idx_un_seau(void)
{
    check_index(0.3, 1, 0);
}

static void idx_mille(void)
{
    check_index(0.123456, 1000, 123);
}

/* ---------------- Étape 2 · bucket_push ---------------- */

/* Seau gardé accessible : sa zone ne compte pas comme une fuite si le test échoue. */
static struct bucket kept;

static void check_bucket(const char *what, size_t exp_len, size_t exp_cap,
                         const double *exp_items)
{
    bool ok = kept.len == exp_len && kept.cap == exp_cap && kept.items != NULL;
    for (size_t i = 0; ok && i < exp_len; i++) {
        ok = kept.items[i] == exp_items[i];
    }
    if (ok) {
        return;
    }
    tl_line("%s", what);
    tl_line("attendu : len = %zu, cap = %zu", exp_len, exp_cap);
    tl_line("obtenu  : len = %zu, cap = %zu, items %s", kept.len, kept.cap,
            kept.items == NULL ? "= NULL" : "alloué");
    if (kept.len == 0 && kept.items == NULL) {
        tl_symptom("le seau est resté vide.");
        tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : mets-tu à jour "
                    "les champs du seau pointé par b ?");
    } else if (kept.len != exp_len) {
        tl_symptom("len vaut %zu au lieu de %zu.", kept.len, exp_len);
        tl_question("Augmentes-tu len à chaque ajout ?");
    } else if (kept.cap != exp_cap) {
        tl_symptom("la capacité vaut %zu au lieu de %zu.", kept.cap, exp_cap);
        tl_question("Quand le seau est plein, que devient la capacité ? Et quand elle "
                    "valait 0 ?");
    } else {
        size_t i = 0;
        while (kept.items[i] == exp_items[i]) {
            i++;
        }
        show_doubles("attendu", exp_items, exp_len, i);
        show_doubles("obtenu", kept.items, kept.len, i);
        tl_symptom("items[%zu] vaut %g au lieu de %g.", i, kept.items[i], exp_items[i]);
        tl_question("Après realloc, utilises-tu bien la nouvelle zone ? Écris-tu x à "
                    "l'indice len ?");
    }
    tl_fail();
}

static void push_and_check(double x)
{
    tl_on_crash("Écris-tu dans une case au-delà de la capacité ? Quelle taille en octets "
                "demandes-tu à realloc ?");
    int ret = bucket_push(&kept, x);
    if (ret != 0) {
        tl_line("bucket_push(seau, %g)", x);
        tl_symptom("bucket_push renvoie %d alors que realloc a réussi.", ret);
        tl_question("Que doit renvoyer ta fonction quand tout s'est bien passé ?");
        tl_fail();
    }
}

static void push_premier(void)
{
    static const double expected[] = { 0.5 };
    push_and_check(0.5);
    check_bucket("ajout de 0.5 dans un seau vide", 1, 4, expected);
    free(kept.items);
}

static void push_cinq(void)
{
    static const double expected[] = { 0.1, 0.2, 0.3, 0.4, 0.5 };
    for (size_t i = 0; i < 5; i++) {
        push_and_check(expected[i]);
    }
    check_bucket("5 ajouts dans un seau vide", 5, 8, expected);
    free(kept.items);
}

static void push_existant(void)
{
    static const double expected[] = { 0.7, 0.8, 0.9 };
    kept.items = malloc(2 * sizeof(double));
    if (kept.items == NULL) {
        return;
    }
    kept.items[0] = 0.7;
    kept.items[1] = 0.8;
    kept.len = 2;
    kept.cap = 2;
    push_and_check(0.9);
    check_bucket("ajout de 0.9 dans un seau plein {0.7, 0.8} de capacité 2", 3, 4, expected);
    free(kept.items);
}

static void push_mille(void)
{
    enum { N = 1000 };
    double *expected = doubles_new(N);
    for (size_t i = 0; i < N; i++) {
        expected[i] = (double)i / N;
        push_and_check(expected[i]);
    }
    check_bucket("1000 ajouts dans un seau vide", N, 1024, expected);
    free(kept.items);
    doubles_free(expected);
}

/* ---------------- Étape 3 · bucket_sort ---------------- */

static void check_sort(const double *in, size_t n)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta fonction alloue-t-elle ou lit-elle quand même ?");
        int ret = bucket_sort(NULL, 0);
        if (ret != 0) {
            tl_line("n = 0 avec a = NULL");
            tl_symptom("bucket_sort renvoie %d pour un tableau vide.", ret);
            tl_question("Un tableau vide est-il une erreur ?");
            tl_fail();
        }
        return;
    }
    double *a = doubles_new(n);
    double *expected = doubles_new(n);
    memcpy(a, in, n * sizeof(double));
    memcpy(expected, in, n * sizeof(double));
    qsort(expected, n, sizeof(double), cmp_double);
    tl_on_crash("Les numéros de seau restent-ils < n ? Chaque seau est-il bien initialisé "
                "(items NULL, len et cap à 0) ?");
    int ret = bucket_sort(a, n);
#if TL_GUARDS
    if (a[-1] != DGUARD || a[n] != DGUARD) {
        tl_symptom("bucket_sort a écrit juste %s du tableau.",
                   a[-1] != DGUARD ? "avant le début" : "après la fin");
        tl_question("En recopiant les seaux dans a, combien de valeurs écris-tu ?");
        tl_fail();
    }
#endif
    size_t i = 0;
    while (i < n && a[i] == expected[i]) {
        i++;
    }
    if (ret == 0 && i == n) {
        doubles_free(a);
        doubles_free(expected);
        return;
    }
    if (n <= TL_SHOW_MAX) {
        show_doubles("entrée", in, n, 0);
    }
    show_doubles("attendu", expected, n, i < n ? i : 0);
    show_doubles("obtenu", a, n, i < n ? i : 0);
    if (ret != 0) {
        tl_symptom("bucket_sort renvoie %d alors que les allocations ont réussi.", ret);
        tl_question("Que doit renvoyer ta fonction quand tout s'est bien passé ?");
        tl_fail();
    }
    bool same = memcmp(a, in, n * sizeof(double)) == 0;
    double *sorted_got = doubles_new(n);
    memcpy(sorted_got, a, n * sizeof(double));
    qsort(sorted_got, n, sizeof(double), cmp_double);
    bool perm = memcmp(sorted_got, expected, n * sizeof(double)) == 0;
    if (same) {
        tl_symptom("le tableau n'a pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 3, c'est normal. Sinon : recopies-tu "
                    "le contenu des seaux dans a ?");
    } else if (!perm) {
        tl_symptom("des valeurs ont été perdues ou dupliquées.");
        tl_question("Chaque valeur va-t-elle dans exactement un seau ? Recopies-tu chaque "
                    "seau en entier, à la suite du précédent ?");
    } else {
        tl_symptom("toutes les valeurs sont là, mais a[%zu] = %g n'est pas à sa place.", i,
                   a[i]);
        tl_question("Chaque seau est-il trié avant d'être recopié ? Les seaux sont-ils "
                    "recopiés dans l'ordre de leur numéro ?");
    }
    tl_fail();
}

static void sort_vide(void)
{
    check_sort(NULL, 0);
}

static void sort_un(void)
{
    static const double in[] = { 0.42 };
    check_sort(in, 1);
}

static void sort_deux(void)
{
    static const double in[] = { 0.9, 0.1 };
    check_sort(in, 2);
}

static void sort_trie(void)
{
    static const double in[] = { 0.0, 0.1, 0.25, 0.5, 0.75, 0.9 };
    check_sort(in, 6);
}

static void sort_inverse(void)
{
    static const double in[] = { 0.95, 0.8, 0.61, 0.4, 0.33, 0.2, 0.01 };
    check_sort(in, 7);
}

static void sort_meme_seau_local(void)
{
    static const double in[] = { 0.17, 0.11, 0.15, 0.12, 0.9 };
    check_sort(in, 5);
}

static void sort_doublons(void)
{
    static const double in[] = { 0.5, 0.3, 0.5, 0.1, 0.3, 0.5 };
    check_sort(in, 6);
}

static void sort_egaux(void)
{
    static const double in[] = { 0.7, 0.7, 0.7, 0.7 };
    check_sort(in, 4);
}

static void sort_bornes(void)
{
    static const double in[] = { 0.9999999999999999, 0.0, 0.5, 0.0, 0.9999999999999999 };
    check_sort(in, 5);
}

static void check_random(size_t n, uint64_t seed, double scale)
{
    double *in = doubles_new(n);
    tl_rand_seed(seed);
    for (size_t i = 0; i < n; i++) {
        in[i] = tl_rand_unit() * scale;
    }
    check_sort(in, n);
    doubles_free(in);
}

static void sort_aleatoire(void)
{
    check_random(1000, 1515, 1.0);
}

static void sort_grand(void)
{
    tl_on_timeout("Utilises-tu bien n seaux ? Le travail par seau reste-t-il petit ?");
    check_random(1000000, 1516, 1.0);
}

static void sort_tous_dans_un_seau(void)
{
    check_random(2000, 1517, 0.001);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "bucket_index", false },
    { 2, "bucket_push", false },
    { 3, "bucket_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, idx_zero, "x = 0, k = 10"),
    TL_TEST(1, idx_dixieme, "x = 0.05, k = 10"),
    TL_TEST(1, idx_moitie, "x = 0.5, k = 4"),
    TL_TEST(1, idx_quart, "x = 0.25, k = 4"),
    TL_TEST(1, idx_presque_un, "x = 0.99, k = 10"),
    TL_TEST(1, idx_juste_sous_un, "x juste sous 1, k = 7"),
    TL_TEST(1, idx_un_seau, "un seul seau"),
    TL_TEST(1, idx_mille, "x = 0.123456, k = 1000"),

    TL_TEST(2, push_premier, "premier ajout"),
    TL_TEST(2, push_cinq, "5 ajouts : la capacité passe de 4 à 8"),
    TL_TEST(2, push_existant, "ajout dans un seau plein de capacité 2"),
    TL_TEST(2, push_mille, "1000 ajouts"),

    TL_TEST(3, sort_vide, "n = 0 avec a = NULL"),
    TL_TEST(3, sort_un, "1 élément"),
    TL_TEST(3, sort_deux, "2 éléments inversés"),
    TL_TEST(3, sort_trie, "déjà trié"),
    TL_TEST(3, sort_inverse, "trié à l'envers"),
    TL_TEST(3, sort_meme_seau_local, "plusieurs valeurs dans le même seau"),
    TL_TEST(3, sort_doublons, "doublons"),
    TL_TEST(3, sort_egaux, "tous égaux"),
    TL_TEST(3, sort_bornes, "0 et une valeur juste sous 1"),
    TL_TEST(3, sort_aleatoire, "1000 valeurs aléatoires"),
    TL_TEST(3, sort_grand, "1 000 000 valeurs aléatoires"),
    TL_TEST(3, sort_tous_dans_un_seau, "2000 valeurs dans [0, 0.001)"),
};

TL_MAIN("15", "Tri par seaux", STEPS, TESTS)

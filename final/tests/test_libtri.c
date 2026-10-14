/* Projet final — tests de libtri. NE PAS MODIFIER CE FICHIER. */
#include "libtri.h"
#include "tl_check.h"

#include <math.h>

/* Table de référence, dans l'ordre de l'énumération. */
static const char *const NAMES[TRI_ALGO_COUNT] = {
    "bulle", "selection", "insertion", "cocktail", "peigne", "shell", "fusion", "rapide",
    "abr", "avl", "tas", "comptage", "radix", "introsort", "timsort",
};

static const bool STABLE[TRI_ALGO_COUNT] = {
    true, false, true, true, false, false, true, false,
    true, true, false, true, true, false, true,
};

static const char *const STATUS_NAMES[] = {
    "TRI_OK", "TRI_EINVAL", "TRI_ENOMEM", "TRI_EUNSUPPORTED",
};

static const char *status_name(tri_status s)
{
    return (unsigned)s < 4 ? STATUS_NAMES[s] : "(valeur inconnue)";
}

/* Comparateurs du test. */
static int t_cmp_int(const void *x, const void *y)
{
    int a = *(const int *)x;
    int b = *(const int *)y;
    return (a > b) - (a < b);
}

static int t_cmp_int_desc(const void *x, const void *y)
{
    return t_cmp_int(y, x);
}

static int t_cmp_double(const void *x, const void *y)
{
    double a = *(const double *)x;
    double b = *(const double *)y;
    return (a > b) - (a < b);
}

/* Un élément avec une clé et son rang d'origine : sert à voir la stabilité. */
struct rec {
    int key;
    unsigned origin;
};

static int t_cmp_rec(const void *x, const void *y)
{
    return t_cmp_int(&((const struct rec *)x)->key, &((const struct rec *)y)->key);
}

static int t_cmp_rec_origin(const void *x, const void *y)
{
    int c = t_cmp_rec(x, y);
    if (c != 0) {
        return c;
    }
    unsigned a = ((const struct rec *)x)->origin;
    unsigned b = ((const struct rec *)y)->origin;
    return (a > b) - (a < b);
}

/* Un gros élément : une clé et 60 octets qui doivent la suivre. */
struct big {
    int key;
    unsigned char payload[60];
};

static int t_cmp_big(const void *x, const void *y)
{
    return t_cmp_int(&((const struct big *)x)->key, &((const struct big *)y)->key);
}

static void expect_status(const char *call, tri_status got, tri_status expected)
{
    if (got == expected) {
        return;
    }
    tl_line("appel   : %s", call);
    tl_line("attendu : %s", status_name(expected));
    tl_line("obtenu  : %s", status_name(got));
    if (expected == TRI_OK) {
        tl_symptom("l'appel devait réussir.");
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : dans quel "
                    "cas ta fonction renvoie-t-elle %s ?", status_name(got));
    } else {
        tl_symptom("cet appel est invalide (voir include/libtri.h), et ta fonction ne le "
                   "signale pas.");
        tl_question("Quelles vérifications fais-tu sur les arguments avant de trier ?");
    }
    tl_fail();
}

/* ---------------- Étape 1 · tri_algo_name, tri_algo_is_stable ---------------- */

static void infos_noms(void)
{
    for (int i = 0; i < TRI_ALGO_COUNT; i++) {
        const char *got = tri_algo_name((tri_algo)i);
        if (got == NULL || strcmp(got, NAMES[i]) != 0) {
            tl_line("tri_algo_name(%d)", i);
            tl_line("attendu : \"%s\"", NAMES[i]);
            tl_line("obtenu  : %s%s%s", got ? "\"" : "", got ? got : "NULL", got ? "\"" : "");
            tl_symptom("le nom ne correspond pas à la liste de include/libtri.h.");
            tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : ta table "
                        "des noms suit-elle l'ordre de l'énumération ?");
            tl_fail();
        }
    }
}

static void infos_nom_invalide(void)
{
    const char *got = tri_algo_name(TRI_ALGO_COUNT);
    if (got != NULL) {
        tl_line("tri_algo_name(TRI_ALGO_COUNT)");
        tl_line("attendu : NULL");
        tl_line("obtenu  : \"%s\"", got);
        tl_symptom("TRI_ALGO_COUNT n'est pas un algorithme.");
        tl_question("Vérifies-tu que algo est bien dans [0, TRI_ALGO_COUNT) ?");
        tl_fail();
    }
}

static void infos_stabilite(void)
{
    for (int i = 0; i < TRI_ALGO_COUNT; i++) {
        bool got = tri_algo_is_stable((tri_algo)i);
        if (got != STABLE[i]) {
            tl_line("tri_algo_is_stable(%s)", NAMES[i]);
            tl_line("attendu : %s", STABLE[i] ? "true" : "false");
            tl_line("obtenu  : %s", got ? "true" : "false");
            tl_symptom("la stabilité ne correspond pas à la liste de include/libtri.h.");
            tl_question("Ce tri garde-t-il l'ordre de deux éléments équivalents ? Relis le "
                        "tableau récapitulatif de docs/complexite.md.");
            tl_fail();
        }
    }
    if (tri_algo_is_stable(TRI_ALGO_COUNT)) {
        tl_line("tri_algo_is_stable(TRI_ALGO_COUNT)");
        tl_symptom("TRI_ALGO_COUNT n'est pas un algorithme : la réponse doit être false.");
        tl_question("Vérifies-tu que algo est bien dans [0, TRI_ALGO_COUNT) ?");
        tl_fail();
    }
}

/* ---------------- Étape 2 · vérification des arguments ---------------- */

static void err_base_null(void)
{
    tl_on_crash("Vérifies-tu base avant de commencer à trier ?");
    expect_status("tri_sort(NULL, 5, sizeof(int), cmp, TRI_MERGE)",
                  tri_sort(NULL, 5, sizeof(int), t_cmp_int, TRI_MERGE), TRI_EINVAL);
    expect_status("tri_sort(NULL, 0, sizeof(int), cmp, TRI_MERGE)",
                  tri_sort(NULL, 0, sizeof(int), t_cmp_int, TRI_MERGE), TRI_OK);
}

static void check_unchanged_call(const char *call, tri_status got, tri_status expected,
                                 const int *a, const int *in, size_t n)
{
    expect_status(call, got, expected);
    if (!tl_ints_equal(a, in, n)) {
        tl_line("appel   : %s", call);
        tl_symptom("l'appel est refusé, mais le tableau a été modifié.");
        tl_question("Fais-tu toutes les vérifications avant de toucher au tableau ?");
        tl_fail();
    }
}

static void err_tri_sort(void)
{
    static const int in[] = { 3, 1, 2 };
    int a[3] = { 3, 1, 2 };
    tl_on_crash("Vérifies-tu size, cmp et algo avant de commencer à trier ?");
    check_unchanged_call("tri_sort(a, 3, 0, cmp, TRI_MERGE)",
                         tri_sort(a, 3, 0, t_cmp_int, TRI_MERGE), TRI_EINVAL, a, in, 3);
    check_unchanged_call("tri_sort(a, 3, sizeof(int), NULL, TRI_MERGE)",
                         tri_sort(a, 3, sizeof(int), NULL, TRI_MERGE), TRI_EINVAL, a, in, 3);
    check_unchanged_call("tri_sort(a, 3, sizeof(int), cmp, TRI_ALGO_COUNT)",
                         tri_sort(a, 3, sizeof(int), t_cmp_int, TRI_ALGO_COUNT), TRI_EINVAL,
                         a, in, 3);
    check_unchanged_call("tri_sort(a, 3, sizeof(int), cmp, TRI_COUNTING)",
                         tri_sort(a, 3, sizeof(int), t_cmp_int, TRI_COUNTING),
                         TRI_EUNSUPPORTED, a, in, 3);
    check_unchanged_call("tri_sort(a, 3, sizeof(int), cmp, TRI_RADIX)",
                         tri_sort(a, 3, sizeof(int), t_cmp_int, TRI_RADIX), TRI_EUNSUPPORTED,
                         a, in, 3);
}

static void err_tri_sort_int(void)
{
    static const int in[] = { 3, 1, 2 };
    int a[3] = { 3, 1, 2 };
    tl_on_crash("Vérifies-tu a et algo avant de commencer à trier ?");
    expect_status("tri_sort_int(NULL, 3, TRI_MERGE)", tri_sort_int(NULL, 3, TRI_MERGE),
                  TRI_EINVAL);
    expect_status("tri_sort_int(NULL, 0, TRI_MERGE)", tri_sort_int(NULL, 0, TRI_MERGE),
                  TRI_OK);
    check_unchanged_call("tri_sort_int(a, 3, TRI_ALGO_COUNT)",
                         tri_sort_int(a, 3, TRI_ALGO_COUNT), TRI_EINVAL, a, in, 3);
}

static void err_seaux(void)
{
    double a[3] = { 0.5, 1.0, 0.25 };
    double b[3] = { 0.5, -0.1, 0.25 };
    double c[2] = { 0.5, NAN };
    tl_on_crash("Vérifies-tu toutes les valeurs avant de commencer à trier ?");
    expect_status("tri_sort_unit_doubles(NULL, 3)", tri_sort_unit_doubles(NULL, 3),
                  TRI_EINVAL);
    expect_status("tri_sort_unit_doubles(NULL, 0)", tri_sort_unit_doubles(NULL, 0), TRI_OK);
    expect_status("tri_sort_unit_doubles({0.5, 1.0, 0.25}, 3)", tri_sort_unit_doubles(a, 3),
                  TRI_EINVAL);
    expect_status("tri_sort_unit_doubles({0.5, -0.1, 0.25}, 3)", tri_sort_unit_doubles(b, 3),
                  TRI_EINVAL);
    expect_status("tri_sort_unit_doubles({0.5, NAN}, 2)", tri_sort_unit_doubles(c, 2),
                  TRI_EINVAL);
    if (a[0] != 0.5 || a[1] != 1.0 || a[2] != 0.25) {
        tl_line("appel   : tri_sort_unit_doubles({0.5, 1.0, 0.25}, 3)");
        tl_symptom("l'appel est refusé, mais le tableau a été modifié.");
        tl_question("Vérifies-tu toutes les valeurs avant de toucher au tableau ?");
        tl_fail();
    }
}

/* ---------------- Étape 3 · tri_swap ---------------- */

/* tri_swap est interne : on le déclare ici, comme dans src/internal.h. */
void tri_swap(void *x, void *y, size_t size);

static void check_swap(size_t size)
{
    unsigned char *zone = tl_malloc(2 * size + 4);
    unsigned char *x = zone + 1;
    unsigned char *y = zone + size + 3;
    memset(zone, 0xA5, 2 * size + 4);
    for (size_t i = 0; i < size; i++) {
        x[i] = (unsigned char)(i * 7 + 1);
        y[i] = (unsigned char)(200 - i * 3);
    }
    tl_on_crash("Combien d'octets lis-tu et écris-tu ?");
    tri_swap(x, y, size);
    bool ok = x[-1] == 0xA5 && x[size] == 0xA5 && y[-1] == 0xA5 && y[size] == 0xA5;
    for (size_t i = 0; ok && i < size; i++) {
        ok = x[i] == (unsigned char)(200 - i * 3) && y[i] == (unsigned char)(i * 7 + 1);
    }
    if (!ok) {
        tl_line("tri_swap sur deux blocs de %zu octets", size);
        tl_symptom("les deux blocs ne sont pas exactement échangés (ou un octet voisin a "
                   "changé).");
        tl_question("Si tu n'as pas commencé l'étape 3, c'est normal. Sinon : reprends ton "
                    "generic_swap de l'exercice 16. Échanges-tu exactement size octets ?");
        tl_fail();
    }
    tl_free(zone);
}

static void swap_tailles(void)
{
    check_swap(1);
    check_swap(sizeof(int));
    check_swap(sizeof(double));
    check_swap(sizeof(struct big));
    check_swap(1000);
}

static void swap_meme_zone(void)
{
    struct big b = { 42, { 0 } };
    memset(b.payload, 9, sizeof b.payload);
    struct big copy = b;
    tri_swap(&b, &b, sizeof b);
    if (memcmp(&b, &copy, sizeof b) != 0) {
        tl_line("tri_swap(&x, &x, %zu)", sizeof b);
        tl_symptom("échanger une zone avec elle-même l'a modifiée.");
        tl_question("Ta méthode marche-t-elle quand x et y sont la même adresse ?");
        tl_fail();
    }
}

/* ---------------- Étapes 4 à 18 · un algorithme par étape ---------------- */

static tri_algo cur;
static char cur_call[96];

static const char *call_name(const char *api)
{
    snprintf(cur_call, sizeof cur_call, "%s(…, %s)", api, NAMES[cur]);
    return cur_call;
}

static void set_hints(void)
{
    tl_on_crash("Calcules-tu l'adresse de l'élément i comme (unsigned char *)base + i × size ? "
                "Les cas de base de ta récursion s'arrêtent-ils ?");
    tl_on_timeout("Ton algorithme a-t-il la bonne complexité ? Compare avec ton exercice.");
}

/* Tri d'int par tri_sort (générique) : statut vérifié, résultat vérifié par l'oracle. */
static void via_tri_sort(int *a, size_t n)
{
    tri_status s = tri_sort(a, n, sizeof(int), t_cmp_int, cur);
    if (s != TRI_OK) {
        tl_line("tri_sort(a, %zu, sizeof(int), cmp, %s)", n, NAMES[cur]);
        tl_symptom("tri_sort renvoie %s au lieu de TRI_OK.", status_name(s));
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : ton "
                    "dispatcher appelle-t-il le bon tri pour %s ?", NAMES[cur]);
        tl_fail();
    }
}

static void via_tri_sort_int(int *a, size_t n)
{
    tri_status s = tri_sort_int(a, n, cur);
    if (s != TRI_OK) {
        tl_line("tri_sort_int(a, %zu, %s)", n, NAMES[cur]);
        tl_symptom("tri_sort_int renvoie %s au lieu de TRI_OK.", status_name(s));
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : comment "
                    "tri_sort_int traite-t-il %s ?", NAMES[cur]);
        tl_fail();
    }
}

static void small_cases(tl_sort_int_fn sort, const char *func)
{
    static const int one[] = { 42 };
    static const int two[] = { 2, 1 };
    static const int sorted[] = { -3, 0, 2, 5, 8, 13, 21, 34, 55, 89 };
    static const int reversed[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };
    static const int dups[] = { 5, 3, 5, 1, 3, 5, 0, 1 };
    static const int equal[] = { 7, 7, 7, 7, 7, 7 };
    static const int ext[] = { 0, INT_MAX, -7, INT_MIN, 3, -1, INT_MAX, INT_MIN };
    set_hints();
    tl_on_ub("Ta comparaison ou ton calcul de clé peut-il dépasser les limites d'un int ?");
    tl_check_sort_int(func, sort, NULL, 0, NULL);
    tl_check_sort_int(func, sort, one, TL_LEN(one), NULL);
    tl_check_sort_int(func, sort, two, TL_LEN(two), NULL);
    tl_check_sort_int(func, sort, sorted, TL_LEN(sorted), NULL);
    tl_check_sort_int(func, sort, reversed, TL_LEN(reversed), NULL);
    tl_check_sort_int(func, sort, dups, TL_LEN(dups), NULL);
    tl_check_sort_int(func, sort, equal, TL_LEN(equal), NULL);
    if (cur != TRI_COUNTING) {
        tl_check_sort_int(func, sort, ext, TL_LEN(ext), NULL);
    }
}

static void gen_limites(void)
{
    small_cases(via_tri_sort, call_name("tri_sort"));
}

static void gen_aleatoire(size_t n, uint64_t seed)
{
    set_hints();
    tl_check_sort_random(call_name("tri_sort"), via_tri_sort, n, seed, INT_MIN, INT_MAX);
}

static void gen_api_int(void)
{
    set_hints();
    tl_check_sort_random(call_name("tri_sort_int"), via_tri_sort_int, 1000, 19001, -1000000,
                         1000000);
    small_cases(via_tri_sort_int, call_name("tri_sort_int"));
}

static void gen_decroissant(void)
{
    enum { N = 500 };
    int *in = tl_ints_new(N);
    tl_rand_seed(19002);
    tl_fill_random(in, N, -1000, 1000);
    int *a = tl_ints_copy(in, N);
    int *expected = tl_ints_copy(in, N);
    qsort(expected, N, sizeof(int), t_cmp_int_desc);
    set_hints();
    tri_status s = tri_sort(a, N, sizeof(int), t_cmp_int_desc, cur);
    if (s != TRI_OK || !tl_ints_equal(a, expected, N)) {
        tl_line("tri_sort(a, 500, sizeof(int), comparateur inversé, %s)", NAMES[cur]);
        if (s != TRI_OK) {
            tl_symptom("tri_sort renvoie %s au lieu de TRI_OK.", status_name(s));
        } else {
            size_t i = tl_first_diff(expected, a, N);
            tl_show("attendu", expected, N, i, NULL);
            tl_show("obtenu", a, N, i, NULL);
            tl_symptom("le tableau n'est pas dans l'ordre demandé par le comparateur.");
        }
        tl_question("Ton tri compare-t-il les éléments uniquement avec cmp ?");
        tl_fail();
    }
    tl_ints_free(a);
    tl_ints_free(expected);
    tl_ints_free(in);
}

static void gen_double(void)
{
    enum { N = 500 };
    double *a = tl_malloc(N * sizeof *a);
    double *expected = tl_malloc(N * sizeof *expected);
    tl_rand_seed(19003);
    for (size_t i = 0; i < N; i++) {
        a[i] = (tl_rand_unit() - 0.5) * 1000.0;
        expected[i] = a[i];
    }
    qsort(expected, N, sizeof(double), t_cmp_double);
    set_hints();
    tri_status s = tri_sort(a, N, sizeof(double), t_cmp_double, cur);
    for (size_t i = 0; i < N; i++) {
        if (s != TRI_OK || a[i] != expected[i]) {
            tl_line("tri_sort de 500 double, %s", NAMES[cur]);
            if (s != TRI_OK) {
                tl_symptom("tri_sort renvoie %s au lieu de TRI_OK.", status_name(s));
            } else {
                tl_symptom("a[%zu] vaut %g au lieu de %g.", i, a[i], expected[i]);
            }
            tl_question("Ton tri copie-t-il des éléments de size octets (8 ici), et pas des "
                        "int ?");
            tl_fail();
        }
    }
    tl_free(a);
    tl_free(expected);
}

static void gen_gros(void)
{
    enum { N = 400 };
    struct big *b = tl_malloc(N * sizeof *b);
    tl_rand_seed(19004);
    for (size_t i = 0; i < N; i++) {
        b[i].key = tl_rand_range(-500, 500);
        memset(b[i].payload, b[i].key & 0xFF, sizeof b[i].payload);
    }
    set_hints();
    tri_status s = tri_sort(b, N, sizeof *b, t_cmp_big, cur);
    for (size_t i = 0; i < N; i++) {
        bool intact = true;
        for (size_t j = 0; j < sizeof b[i].payload; j++) {
            intact = intact && b[i].payload[j] == (unsigned char)(b[i].key & 0xFF);
        }
        if (s != TRI_OK || !intact || (i + 1 < N && b[i].key > b[i + 1].key)) {
            tl_line("tri_sort de 400 éléments de %zu octets, %s", sizeof *b, NAMES[cur]);
            if (s != TRI_OK) {
                tl_symptom("tri_sort renvoie %s au lieu de TRI_OK.", status_name(s));
            } else if (!intact) {
                tl_symptom("l'élément %zu a une clé qui ne correspond plus à son contenu.", i);
            } else {
                tl_symptom("les clés ne sont pas dans l'ordre (position %zu).", i);
            }
            tl_question("Ton tri déplace-t-il chaque élément en entier (size octets), y "
                        "compris dans ses tableaux ou nœuds auxiliaires ?");
            tl_fail();
        }
    }
    tl_free(b);
}

static void gen_stabilite(void)
{
    enum { N = 2000 };
    struct rec *a = tl_malloc(N * sizeof *a);
    struct rec *expected = tl_malloc(N * sizeof *expected);
    tl_rand_seed(19005);
    for (size_t i = 0; i < N; i++) {
        a[i].key = tl_rand_range(0, 40);
        a[i].origin = (unsigned)i;
    }
    memcpy(expected, a, N * sizeof *a);
    qsort(expected, N, sizeof *expected, t_cmp_rec_origin);
    set_hints();
    tri_status s = tri_sort(a, N, sizeof *a, t_cmp_rec, cur);
    if (s != TRI_OK) {
        tl_line("tri_sort de 2000 éléments à clés répétées, %s", NAMES[cur]);
        tl_symptom("tri_sort renvoie %s au lieu de TRI_OK.", status_name(s));
        tl_question("Ton dispatcher appelle-t-il le bon tri ?");
        tl_fail();
    }
    for (size_t i = 0; i < N; i++) {
        bool same_key = a[i].key == expected[i].key;
        bool same = same_key && a[i].origin == expected[i].origin;
        if (!same_key || (STABLE[cur] && !same)) {
            tl_line("2000 éléments, clés entre 0 et 40, tri %s (%s)", NAMES[cur],
                    STABLE[cur] ? "déclaré stable" : "instable");
            if (!same_key) {
                tl_symptom("position %zu : clé %d au lieu de %d.", i, a[i].key,
                           expected[i].key);
                tl_question("Tes tests sur les int passent-ils ? Déplaces-tu des éléments "
                            "entiers ?");
            } else {
                tl_symptom("deux éléments de clé %d ont changé d'ordre : celui d'origine "
                           "%u passe avant celui d'origine %u.", a[i].key, a[i].origin,
                           expected[i].origin);
                tl_question("Ce tri est déclaré stable : quand cmp renvoie 0, lequel des deux "
                            "éléments places-tu en premier ?");
            }
            tl_fail();
        }
    }
    tl_free(a);
    tl_free(expected);
}

#define ALGO_TESTS(id, A, BIG)                                                          \
    static void id##_limites(void)                                                      \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_limites();                                                                  \
    }                                                                                   \
    static void id##_aleatoire(void)                                                    \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_aleatoire(1000, 19010u + (uint64_t)(A));                                    \
    }                                                                                   \
    static void id##_api_int(void)                                                      \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_api_int();                                                                  \
    }                                                                                   \
    static void id##_decroissant(void)                                                  \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_decroissant();                                                              \
    }                                                                                   \
    static void id##_double(void)                                                       \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_double();                                                                   \
    }                                                                                   \
    static void id##_gros(void)                                                         \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_gros();                                                                     \
    }                                                                                   \
    static void id##_stabilite(void)                                                    \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_stabilite();                                                                \
    }                                                                                   \
    static void id##_grand(void)                                                        \
    {                                                                                   \
        cur = (A);                                                                      \
        gen_aleatoire((BIG), 19030u + (uint64_t)(A));                                   \
    }

#define ALGO_ENTRIES(step, id)                                                          \
    TL_TEST(step, id##_limites, "cas limites, avec tri_sort"),                          \
    TL_TEST(step, id##_aleatoire, "1000 int aléatoires, avec tri_sort"),                \
    TL_TEST(step, id##_api_int, "avec tri_sort_int"),                                   \
    TL_TEST(step, id##_decroissant, "comparateur inversé"),                             \
    TL_TEST(step, id##_double, "500 double"),                                           \
    TL_TEST(step, id##_gros, "éléments de 64 octets"),                                  \
    TL_TEST(step, id##_stabilite, "clés répétées (stabilité si déclaré stable)"),       \
    TL_TEST(step, id##_grand, "grand tableau aléatoire")

ALGO_TESTS(bulle, TRI_BUBBLE, 3000)
ALGO_TESTS(selection, TRI_SELECTION, 3000)
ALGO_TESTS(insertion, TRI_INSERTION, 3000)
ALGO_TESTS(cocktail, TRI_COCKTAIL, 3000)
ALGO_TESTS(peigne, TRI_COMB, 100000)
ALGO_TESTS(shell, TRI_SHELL, 100000)
ALGO_TESTS(fusion, TRI_MERGE, 200000)
ALGO_TESTS(rapide, TRI_QUICK, 200000)
ALGO_TESTS(abr, TRI_TREE, 100000)
ALGO_TESTS(avl, TRI_AVL, 200000)
ALGO_TESTS(tas, TRI_HEAP, 200000)
ALGO_TESTS(introsort, TRI_INTROSORT, 200000)
ALGO_TESTS(timsort, TRI_TIMSORT, 200000)

/* Tris sans comparaison : seulement par tri_sort_int. */

static void int_limites(tri_algo a)
{
    cur = a;
    small_cases(via_tri_sort_int, call_name("tri_sort_int"));
}

static void int_aleatoire(tri_algo a, int lo, int hi, size_t n)
{
    cur = a;
    set_hints();
    tl_check_sort_random(call_name("tri_sort_int"), via_tri_sort_int, n, 19050 + (uint64_t)a,
                         lo, hi);
}

static void comptage_limites(void)
{
    int_limites(TRI_COUNTING);
}

static void comptage_aleatoire(void)
{
    int_aleatoire(TRI_COUNTING, -1000000, 1000000, 1000);
}

static void comptage_grand(void)
{
    int_aleatoire(TRI_COUNTING, 0, 1000000, 1000000);
}

static void comptage_refus(void)
{
    static const int in1[] = { INT_MAX, 0, INT_MIN };
    static const int in2[] = { 0, TRI_COUNTING_MAX_RANGE, 3 };
    int a[3];
    tl_on_ub("Calcules-tu max - min dans un type assez grand ?");
    memcpy(a, in1, sizeof a);
    check_unchanged_call("tri_sort_int({INT_MAX, 0, INT_MIN}, 3, TRI_COUNTING)",
                         tri_sort_int(a, 3, TRI_COUNTING), TRI_EUNSUPPORTED, a, in1, 3);
    memcpy(a, in2, sizeof a);
    check_unchanged_call("tri_sort_int({0, TRI_COUNTING_MAX_RANGE, 3}, 3, TRI_COUNTING)",
                         tri_sort_int(a, 3, TRI_COUNTING), TRI_EUNSUPPORTED, a, in2, 3);
}

static void radix_limites(void)
{
    int_limites(TRI_RADIX);
}

static void radix_aleatoire(void)
{
    int_aleatoire(TRI_RADIX, INT_MIN, INT_MAX, 1000);
}

static void radix_grand(void)
{
    int_aleatoire(TRI_RADIX, INT_MIN, INT_MAX, 1000000);
}

/* ---------------- Étape 19 · seaux ---------------- */

static void check_unit(const double *in, size_t n, const char *what)
{
    double *a = tl_malloc((n > 0 ? n : 1) * sizeof *a);
    double *expected = tl_malloc((n > 0 ? n : 1) * sizeof *expected);
    if (n > 0) {
        memcpy(a, in, n * sizeof *a);
        memcpy(expected, in, n * sizeof *a);
        qsort(expected, n, sizeof *expected, t_cmp_double);
    }
    set_hints();
    tri_status s = tri_sort_unit_doubles(n > 0 ? a : NULL, n);
    for (size_t i = 0; i < n || s != TRI_OK; i++) {
        if (s != TRI_OK || a[i] != expected[i]) {
            tl_line("tri_sort_unit_doubles : %s", what);
            if (s != TRI_OK) {
                tl_symptom("la fonction renvoie %s au lieu de TRI_OK.", status_name(s));
            } else {
                tl_symptom("a[%zu] vaut %.17g au lieu de %.17g.", i, a[i], expected[i]);
            }
            tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : reprends "
                        "ton tri par seaux de l'exercice 15.");
            tl_fail();
        }
    }
    tl_free(a);
    tl_free(expected);
}

static void seaux_limites(void)
{
    static const double one[] = { 0.42 };
    static const double two[] = { 0.9, 0.1 };
    static const double dups[] = { 0.5, 0.3, 0.5, 0.1, 0.3, 0.5 };
    static const double bounds[] = { 0.9999999999999999, 0.0, 0.5, 0.0 };
    check_unit(NULL, 0, "tableau vide");
    check_unit(one, 1, "1 valeur");
    check_unit(two, 2, "{0.9, 0.1}");
    check_unit(dups, 6, "doublons");
    check_unit(bounds, 4, "0 et une valeur juste sous 1");
}

static void check_unit_random(size_t n, uint64_t seed, const char *what)
{
    double *in = tl_malloc(n * sizeof *in);
    tl_rand_seed(seed);
    for (size_t i = 0; i < n; i++) {
        in[i] = tl_rand_unit();
    }
    check_unit(in, n, what);
    tl_free(in);
}

static void seaux_aleatoire(void)
{
    check_unit_random(1000, 19060, "1000 valeurs aléatoires");
}

static void seaux_grand(void)
{
    check_unit_random(1000000, 19061, "1 000 000 valeurs aléatoires");
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "tri_algo_name, tri_algo_is_stable (libtri.c)", false },
    { 2, "vérification des arguments (libtri.c)", false },
    { 3, "tri_swap (common.c)", false },
    { 4, "bulle (bubble.c)", false },
    { 5, "selection (selection.c)", false },
    { 6, "insertion (insertion.c)", false },
    { 7, "cocktail (cocktail.c)", false },
    { 8, "peigne (comb.c)", false },
    { 9, "shell (shell.c)", false },
    { 10, "fusion (merge.c)", false },
    { 11, "rapide (quick.c)", false },
    { 12, "abr (tree.c)", false },
    { 13, "avl (avl.c)", false },
    { 14, "tas (heap.c)", false },
    { 15, "comptage (counting.c)", false },
    { 16, "radix (radix.c)", false },
    { 17, "introsort (introsort.c)", false },
    { 18, "timsort (timsort.c)", false },
    { 19, "seaux (bucket.c)", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, infos_noms, "les 15 noms"),
    TL_TEST(1, infos_nom_invalide, "nom d'un algorithme invalide"),
    TL_TEST(1, infos_stabilite, "stabilité des 15 algorithmes"),

    TL_TEST(2, err_base_null, "tri_sort avec base = NULL"),
    TL_TEST(2, err_tri_sort, "tri_sort : size, cmp, algo"),
    TL_TEST(2, err_tri_sort_int, "tri_sort_int : a, algo"),
    TL_TEST(2, err_seaux, "tri_sort_unit_doubles : a, valeurs hors de [0, 1)"),

    TL_TEST(3, swap_tailles, "blocs de 1 à 1000 octets"),
    TL_TEST(3, swap_meme_zone, "une zone avec elle-même"),

    ALGO_ENTRIES(4, bulle),
    ALGO_ENTRIES(5, selection),
    ALGO_ENTRIES(6, insertion),
    ALGO_ENTRIES(7, cocktail),
    ALGO_ENTRIES(8, peigne),
    ALGO_ENTRIES(9, shell),
    ALGO_ENTRIES(10, fusion),
    ALGO_ENTRIES(11, rapide),
    ALGO_ENTRIES(12, abr),
    ALGO_ENTRIES(13, avl),
    ALGO_ENTRIES(14, tas),

    TL_TEST(15, comptage_limites, "cas limites, avec tri_sort_int"),
    TL_TEST(15, comptage_aleatoire, "1000 int entre -1 000 000 et 1 000 000"),
    TL_TEST(15, comptage_grand, "1 000 000 int entre 0 et 1 000 000"),
    TL_TEST(15, comptage_refus, "plage trop grande : TRI_EUNSUPPORTED"),

    TL_TEST(16, radix_limites, "cas limites, avec tri_sort_int"),
    TL_TEST(16, radix_aleatoire, "1000 int aléatoires"),
    TL_TEST(16, radix_grand, "1 000 000 int aléatoires"),

    ALGO_ENTRIES(17, introsort),
    ALGO_ENTRIES(18, timsort),

    TL_TEST(19, seaux_limites, "cas limites"),
    TL_TEST(19, seaux_aleatoire, "1000 valeurs aléatoires"),
    TL_TEST(19, seaux_grand, "1 000 000 valeurs aléatoires"),
};

TL_MAIN("19", "Projet final : libtri", STEPS, TESTS)

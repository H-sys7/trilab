/* Exercice 14 — tests. NE PAS MODIFIER CE FICHIER. */
#include "radix.h"
#include "tl_check.h"

/* ---------------- Étape 1 · radix_byte ---------------- */

static void check_bytes(int x, const char *what, const unsigned expected[4])
{
    unsigned got[4];
    tl_on_ub("Décales-tu un int négatif ? Convertis-tu d'abord x en non signé ?");
    for (unsigned b = 0; b < 4; b++) {
        got[b] = radix_byte(x, b);
    }
    if (got[0] == expected[0] && got[1] == expected[1] && got[2] == expected[2]
        && got[3] == expected[3]) {
        return;
    }
    tl_line("x = %s", what);
    tl_line("attendu : octets 0 à 3 = 0x%02X 0x%02X 0x%02X 0x%02X", expected[0], expected[1],
            expected[2], expected[3]);
    tl_line("obtenu  : octets 0 à 3 = 0x%02X 0x%02X 0x%02X 0x%02X", got[0], got[1], got[2],
            got[3]);
    if (got[0] == 0 && got[1] == 0 && got[2] == 0 && got[3] == 0) {
        tl_symptom("la fonction renvoie 0 pour chaque octet.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : de combien de "
                    "bits faut-il décaler la clé pour amener l'octet b tout à droite ?");
    } else if (got[0] > 255 || got[1] > 255 || got[2] > 255 || got[3] > 255) {
        tl_symptom("un résultat dépasse 255 : il contient plus d'un octet.");
        tl_question("Après le décalage, quel masque ne garde que les 8 bits de droite ?");
    } else if (got[0] == expected[3] && got[3] == expected[0] && got[1] == expected[2]) {
        tl_symptom("les octets sont dans l'ordre inverse : l'octet 0 doit être celui de poids "
                   "faible.");
        tl_question("Pour b = 0, de combien de bits décales-tu ?");
    } else if (got[0] == expected[0] && got[1] == expected[1] && got[2] == expected[2]
               && (got[3] ^ 0x80u) == expected[3]) {
        tl_symptom("seul l'octet 3 est faux, d'exactement 0x80 : le bit de signe n'a pas "
                   "été inversé.");
        tl_question("Sans cette inversion, un nombre négatif aurait-il une clé plus petite ou "
                    "plus grande qu'un positif ? Quel opérateur inverse un seul bit ?");
    } else {
        tl_symptom("certains octets ne correspondent pas.");
        tl_question("Écris la clé de x en hexadécimal, puis découpe-la en 4 octets : "
                    "lequel est l'octet 0 ?");
    }
    tl_fail();
}

static void byte_positif(void)
{
    static const unsigned e[4] = { 0x78, 0x56, 0x34, 0x92 };
    check_bytes(0x12345678, "0x12345678 (clé 0x92345678)", e);
}

static void byte_petit(void)
{
    static const unsigned e[4] = { 0x02, 0x01, 0x00, 0x80 };
    check_bytes(258, "258 = 0x102 (clé 0x80000102)", e);
}

static void byte_zero(void)
{
    static const unsigned e[4] = { 0x00, 0x00, 0x00, 0x80 };
    check_bytes(0, "0 (clé 0x80000000)", e);
}

static void byte_moins_un(void)
{
    static const unsigned e[4] = { 0xFF, 0xFF, 0xFF, 0x7F };
    check_bytes(-1, "-1 (clé 0x7FFFFFFF)", e);
}

static void byte_negatif(void)
{
    static const unsigned e[4] = { 0xFE, 0xFE, 0xFF, 0x7F };
    check_bytes(-258, "-258 (clé 0x7FFFFEFE)", e);
}

static void byte_int_min(void)
{
    static const unsigned e[4] = { 0x00, 0x00, 0x00, 0x00 };
    check_bytes(INT_MIN, "INT_MIN (clé 0x00000000)", e);
}

static void byte_int_max(void)
{
    static const unsigned e[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    check_bytes(INT_MAX, "INT_MAX (clé 0xFFFFFFFF)", e);
}

/* ---------------- Étape 2 · radix_pass ---------------- */

#define OUT_MARK (-77777)

static void check_pass(const int *in, size_t n, unsigned b, const int *expected,
                       const int *unstable)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta fonction lit-elle ou écrit-elle quand même une case ?");
        radix_pass(NULL, NULL, 0, b);
        return;
    }
    int *src = tl_ints_copy(in, n);
    int *out = tl_ints_new(n);
    for (size_t i = 0; i < n; i++) {
        out[i] = OUT_MARK;
    }
    tl_on_crash("Ta table de comptage a-t-elle une case pour chacun des 256 octets "
                "possibles ? Les positions calculées restent-elles dans [0, n) ?");
    radix_pass(src, out, n, b);
    tl_check_guards(src, n, "radix_pass (dans in)");
    tl_check_guards(out, n, "radix_pass (dans out)");
    if (!tl_ints_equal(src, in, n)) {
        tl_symptom("in a été modifié, alors que la passe doit seulement le lire.");
        tl_question("Dans quel tableau écris-tu les valeurs rangées ?");
        tl_fail();
    }
    if (tl_ints_equal(out, expected, n)) {
        tl_ints_free(src);
        tl_ints_free(out);
        return;
    }
    tl_line("passe sur l'octet %u", b);
    tl_report_ints(in, expected, out, n, NULL, NULL);
    size_t marks = 0;
    for (size_t i = 0; i < n; i++) {
        if (out[i] == OUT_MARK) {
            marks++;
        }
    }
    if (marks == n) {
        tl_symptom("out n'a pas été rempli.");
        tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : écris-tu "
                    "bien dans out ?");
    } else if (unstable != NULL && tl_ints_equal(out, unstable, n)) {
        tl_symptom("les valeurs sont rangées par octet, mais deux valeurs de même octet ont "
                   "changé d'ordre : la passe n'est pas stable.");
        tl_question("Quand tu places les valeurs dans out, parcours-tu in dans le sens qui "
                    "garde l'ordre des valeurs de même octet ?");
    } else if (marks > 0) {
        tl_symptom("%zu case(s) de out n'ont pas été remplies.", marks);
        tl_question("Les positions de départ de chaque octet (sommes des comptes "
                    "précédents) sont-elles justes ?");
    } else {
        tl_symptom_diff(in, expected, out, n);
        tl_question("Pour chaque octet possible, à quelle position de out commence-t-il ? "
                    "Avances-tu cette position après chaque valeur placée ?");
    }
    tl_fail();
}

static void pass_vide(void)
{
    check_pass(NULL, 0, 0, NULL, NULL);
}

static void pass_un(void)
{
    static const int in[] = { 7 };
    check_pass(in, 1, 0, in, NULL);
}

static void pass_exemple(void)
{
    static const int in[] = { 0x103, 0x201, 0x302, 0x101 };
    static const int expected[] = { 0x201, 0x101, 0x302, 0x103 };
    static const int unstable[] = { 0x101, 0x201, 0x302, 0x103 };
    check_pass(in, 4, 0, expected, unstable);
}

static void pass_octet_1(void)
{
    static const int in[] = { 0x0102, 0x0201, 0x0101 };
    static const int expected[] = { 0x0102, 0x0101, 0x0201 };
    static const int unstable[] = { 0x0101, 0x0102, 0x0201 };
    check_pass(in, 3, 1, expected, unstable);
}

static void pass_signe(void)
{
    static const int in[] = { -1, 1, INT_MIN, 0 };
    static const int expected[] = { INT_MIN, -1, 1, 0 };
    static const int unstable[] = { INT_MIN, -1, 0, 1 };
    check_pass(in, 4, 3, expected, unstable);
}

/* 1000 valeurs dont les 10 bits de poids faible valent l'indice d'origine : on vérifie
 * l'ordre des octets 2 et la stabilité. */
static void pass_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1414);
    for (size_t i = 0; i < N; i++) {
        unsigned high = (unsigned)tl_rand_range(0, 0x1FFFFF);
        in[i] = (int)((high << 10) | (unsigned)i);
    }
    int *out = tl_ints_new(N);
    radix_pass(in, out, N, 2);
    tl_perm p = tl_check_permutation(in, out, N);
    if (!p.ok) {
        tl_line("1000 valeurs, passe sur l'octet 2");
        tl_symptom_permutation(p);
        tl_question("Tes petits tests de l'étape 2 passent-ils ?");
        tl_fail();
    }
    for (size_t i = 0; i + 1 < N; i++) {
        unsigned b1 = ((unsigned)out[i] >> 16) & 0xFFu;
        unsigned b2 = ((unsigned)out[i + 1] >> 16) & 0xFFu;
        if (b1 > b2 || (b1 == b2 && (out[i] & 1023) > (out[i + 1] & 1023))) {
            tl_line("1000 valeurs, passe sur l'octet 2");
            if (b1 > b2) {
                tl_symptom("out[%zu] a un octet 2 plus grand que out[%zu].", i, i + 1);
                tl_question("Tes petits tests de l'étape 2 passent-ils ?");
            } else {
                tl_symptom("out[%zu] et out[%zu] ont le même octet 2 mais ont changé "
                           "d'ordre : la passe n'est pas stable.", i, i + 1);
                tl_question("Dans quel sens parcours-tu in pour placer les valeurs ?");
            }
            tl_fail();
        }
    }
    tl_ints_free(in);
    tl_ints_free(out);
}

/* ---------------- Étape 3 · radix_sort ---------------- */

TL_SORT_SUITE_DEFINE(radix_sort, 1000000)

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "radix_byte", false },
    { 2, "radix_pass", false },
    { 3, "radix_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, byte_positif, "x = 0x12345678"),
    TL_TEST(1, byte_petit, "x = 258"),
    TL_TEST(1, byte_zero, "x = 0"),
    TL_TEST(1, byte_moins_un, "x = -1"),
    TL_TEST(1, byte_negatif, "x = -258"),
    TL_TEST(1, byte_int_min, "x = INT_MIN"),
    TL_TEST(1, byte_int_max, "x = INT_MAX"),

    TL_TEST(2, pass_vide, "n = 0"),
    TL_TEST(2, pass_un, "1 élément"),
    TL_TEST(2, pass_exemple, "octet 0 de {0x103, 0x201, 0x302, 0x101}"),
    TL_TEST(2, pass_octet_1, "octet 1 de {0x102, 0x201, 0x101}"),
    TL_TEST(2, pass_signe, "octet 3 de {-1, 1, INT_MIN, 0}"),
    TL_TEST(2, pass_aleatoire, "1000 valeurs, octet 2, stabilité"),

    TL_SORT_SUITE_ENTRIES(3, radix_sort),
};

TL_MAIN("14", "Tri par base (radix LSD)", STEPS, TESTS)

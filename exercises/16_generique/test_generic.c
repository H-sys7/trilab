/* Exercice 16 — tests. NE PAS MODIFIER CE FICHIER. */
#include "generic.h"
#include "tl_check.h"

/* Les tests de tri utilisent leurs propres comparateurs : une étape ne dépend pas d'une
 * autre. */
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

static int t_cmp_char(const void *x, const void *y)
{
    return (int)*(const unsigned char *)x - (int)*(const unsigned char *)y;
}

static int t_cmp_double(const void *x, const void *y)
{
    double a = *(const double *)x;
    double b = *(const double *)y;
    return (a > b) - (a < b);
}

static int t_cmp_grade(const void *x, const void *y)
{
    int a = ((const struct student *)x)->grade;
    int b = ((const struct student *)y)->grade;
    return (a > b) - (a < b);
}

static int sign(int v)
{
    return (v > 0) - (v < 0);
}

/* ---------------- Étape 1 · generic_swap ---------------- */

#define GUARD_BYTE 0xA5

/* Échange deux blocs de size octets entourés d'octets sentinelles. */
static void check_swap(const void *x0, const void *y0, size_t size, const char *what)
{
    unsigned char *zone = tl_malloc(2 * size + 4);
    unsigned char *x = zone + 1;
    unsigned char *y = zone + size + 3;
    memset(zone, GUARD_BYTE, 2 * size + 4);
    memcpy(x, x0, size);
    memcpy(y, y0, size);
    tl_on_crash("Combien d'octets lis-tu et écris-tu ? Utilises-tu un tampon de taille fixe "
                "trop petit ?");
    generic_swap(x, y, size);
    bool guards = x[-1] == GUARD_BYTE && x[size] == GUARD_BYTE && y[-1] == GUARD_BYTE
                  && y[size] == GUARD_BYTE;
    if (guards && memcmp(x, y0, size) == 0 && memcmp(y, x0, size) == 0) {
        tl_free(zone);
        return;
    }
    tl_line("échange de %s (%zu octets)", what, size);
    size_t first = 0;
    while (first < size && x[first] == ((const unsigned char *)y0)[first]
           && y[first] == ((const unsigned char *)x0)[first]) {
        first++;
    }
    if (!guards) {
        tl_symptom("un octet juste avant ou juste après l'un des deux blocs a été modifié.");
        tl_question("Ta boucle s'arrête-t-elle après exactement size octets ?");
    } else if (memcmp(x, x0, size) == 0 && memcmp(y, y0, size) == 0) {
        tl_symptom("les deux blocs n'ont pas changé.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : à travers quel "
                    "type de pointeur lis-tu et écris-tu les octets ?");
    } else if (memcmp(x, y, size) == 0) {
        tl_symptom("les deux blocs contiennent maintenant la même chose : un des deux a été "
                   "perdu.");
        tl_question("Où gardes-tu les octets de x pendant que tu écris dans x ?");
    } else if (first > 0) {
        tl_symptom("seuls les %zu premiers octets ont été échangés.", first);
        tl_question("Combien d'octets échanges-tu : size, ou la taille d'un pointeur ?");
    } else {
        tl_symptom("les blocs ne sont pas échangés correctement (premier octet faux : %zu).",
                   first);
        tl_question("Parcours-tu les deux blocs octet par octet, au même rythme ?");
    }
    tl_fail();
}

static void swap_int(void)
{
    int x = 3;
    int y = 7;
    check_swap(&x, &y, sizeof(int), "deux int");
}

static void swap_double(void)
{
    double x = 0.5;
    double y = -2.25;
    check_swap(&x, &y, sizeof(double), "deux double");
}

static void swap_student(void)
{
    struct student x = { "Lina", 12 };
    struct student y = { "Omar", 15 };
    check_swap(&x, &y, sizeof(struct student), "deux struct student");
}

static void swap_char(void)
{
    char x = 'a';
    char y = 'z';
    check_swap(&x, &y, 1, "deux char");
}

static void swap_gros_blocs(void)
{
    enum { N = 1000 };
    unsigned char *x = tl_malloc(N);
    unsigned char *y = tl_malloc(N);
    for (size_t i = 0; i < N; i++) {
        x[i] = (unsigned char)(i % 251);
        y[i] = (unsigned char)(255 - i % 253);
    }
    check_swap(x, y, N, "deux blocs");
    tl_free(x);
    tl_free(y);
}

static void swap_meme_zone(void)
{
    struct student s = { "Ines", 18 };
    struct student copy = s;
    tl_on_crash("Que fait ta fonction quand x et y désignent la même zone ?");
    generic_swap(&s, &s, sizeof s);
    if (memcmp(&s, &copy, sizeof s) != 0) {
        tl_line("échange d'une struct student avec elle-même");
        tl_symptom("le contenu a changé.");
        tl_question("Ta méthode marche-t-elle encore quand x et y sont la même adresse ?");
        tl_fail();
    }
}

static void swap_zero_octet(void)
{
    int x = 1;
    int y = 2;
    tl_on_crash("Avec size == 0, ta boucle tourne-t-elle quand même ?");
    generic_swap(&x, &y, 0);
    if (x != 1 || y != 2) {
        tl_line("échange de 0 octet");
        tl_symptom("les valeurs ont changé alors que size vaut 0.");
        tl_question("Avec size == 0, combien de tours fait ta boucle ?");
        tl_fail();
    }
}

/* ---------------- Étapes 2, 3, 4 · comparateurs ---------------- */

static void check_cmp(const char *func, cmp_fn cmp, const void *x, const void *y,
                      const char *what, int expected_sign)
{
    int got = cmp(x, y);
    if (sign(got) == expected_sign) {
        return;
    }
    static const char *const names[] = { "négatif", "0", "positif" };
    tl_line("%s(%s)", func, what);
    tl_line("attendu : %s", names[expected_sign + 1]);
    tl_line("obtenu  : %d", got);
    if (got == 0 && expected_sign != 0) {
        tl_symptom("le comparateur renvoie 0 : il juge les deux éléments équivalents.");
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : ton calcul "
                    "peut-il perdre une petite différence (conversion en int) ?");
    } else if (expected_sign == 0) {
        tl_symptom("les deux éléments sont équivalents, mais le comparateur ne renvoie pas 0.");
        tl_question("Quels champs ton comparateur regarde-t-il ? Le contrat en demande-t-il "
                    "d'autres ?");
    } else {
        tl_symptom("le signe est inversé ou faux.");
        tl_question("Ton calcul fait-il une soustraction ? Peut-elle dépasser les limites "
                    "d'un int ?");
    }
    tl_fail();
}

static void ci_inferieur(void)
{
    int a = 1;
    int b = 2;
    check_cmp("cmp_int", cmp_int, &a, &b, "1, 2", -1);
}

static void ci_superieur(void)
{
    int a = 2;
    int b = 1;
    check_cmp("cmp_int", cmp_int, &a, &b, "2, 1", 1);
}

static void ci_egal(void)
{
    int a = 5;
    int b = 5;
    check_cmp("cmp_int", cmp_int, &a, &b, "5, 5", 0);
}

static void ci_extremes(void)
{
    int a = INT_MIN;
    int b = INT_MAX;
    tl_on_ub("Ton comparateur fait-il une soustraction ? Que vaut INT_MIN - INT_MAX ?");
    check_cmp("cmp_int", cmp_int, &a, &b, "INT_MIN, INT_MAX", -1);
    check_cmp("cmp_int", cmp_int, &b, &a, "INT_MAX, INT_MIN", 1);
}

static void cd_ordre(void)
{
    double a = 0.1;
    double b = 0.2;
    check_cmp("cmp_double", cmp_double, &a, &b, "0.1, 0.2", -1);
    check_cmp("cmp_double", cmp_double, &b, &a, "0.2, 0.1", 1);
}

static void cd_egal(void)
{
    double a = 1.5;
    double b = 1.5;
    check_cmp("cmp_double", cmp_double, &a, &b, "1.5, 1.5", 0);
}

static void cd_negatif(void)
{
    double a = -2.0;
    double b = 3.0;
    check_cmp("cmp_double", cmp_double, &a, &b, "-2.0, 3.0", -1);
}

static void cd_petit_ecart(void)
{
    double a = 0.3;
    double b = 0.30000000000000004;
    check_cmp("cmp_double", cmp_double, &a, &b, "0.3, 0.30000000000000004", -1);
    check_cmp("cmp_double", cmp_double, &b, &a, "0.30000000000000004, 0.3", 1);
}

static void cs_notes(void)
{
    struct student a = { "Zoe", 10 };
    struct student b = { "Adam", 14 };
    check_cmp("cmp_student", cmp_student, &a, &b, "{Zoe, 10}, {Adam, 14}", -1);
    check_cmp("cmp_student", cmp_student, &b, &a, "{Adam, 14}, {Zoe, 10}", 1);
}

static void cs_meme_note(void)
{
    struct student a = { "Lina", 12 };
    struct student b = { "Omar", 12 };
    check_cmp("cmp_student", cmp_student, &a, &b, "{Lina, 12}, {Omar, 12}", 0);
    check_cmp("cmp_student", cmp_student, &b, &a, "{Omar, 12}, {Lina, 12}", 0);
}

static void cs_extremes(void)
{
    struct student a = { "Min", INT_MIN };
    struct student b = { "Max", INT_MAX };
    tl_on_ub("Ton comparateur fait-il une soustraction ? Que vaut INT_MIN - INT_MAX ?");
    check_cmp("cmp_student", cmp_student, &a, &b, "notes INT_MIN et INT_MAX", -1);
}

/* ---------------- Étapes 5 et 6 · tris génériques ---------------- */

typedef void (*gsort_fn)(void *, size_t, size_t, cmp_fn);

static int merge_ret;

static void merge_wrap(void *base, size_t n, size_t size, cmp_fn cmp)
{
    merge_ret = merge_sort_generic(base, n, size, cmp);
    if (merge_ret != 0) {
        tl_line("%zu éléments de %zu octets", n, size);
        tl_symptom("merge_sort_generic renvoie %d alors que l'allocation a réussi.", merge_ret);
        tl_question("Que doit renvoyer ta fonction quand tout s'est bien passé ?");
        tl_fail();
    }
}

static void set_hints(void)
{
    tl_on_crash("Calcules-tu l'adresse de l'élément i comme base + i × size, sur un pointeur "
                "d'octets ? Copies-tu bien size octets à chaque fois ?");
    tl_on_timeout("Ta boucle d'insertion s'arrête-t-elle quand cmp ne renvoie plus un nombre "
                  "positif ?");
}

/* Tri d'entiers : résultat comparé à qsort. */
static void check_ints(const char *func, gsort_fn sort, const int *in, size_t n, cmp_fn cmp,
                       const char *order)
{
    int *a = n > 0 ? tl_ints_copy(in, n) : NULL;
    set_hints();
    sort(a, n, sizeof(int), cmp);
    if (n == 0) {
        return;
    }
    tl_check_guards(a, n, func);
    int *expected = tl_ints_copy(in, n);
    qsort(expected, n, sizeof(int), cmp);
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        tl_ints_free(expected);
        return;
    }
    tl_line("%zu int, ordre %s", n, order);
    tl_report_ints(in, expected, a, n, NULL, NULL);
    tl_perm p = tl_check_permutation(in, a, n);
    if (tl_ints_equal(a, in, n)) {
        tl_symptom("le tableau n'a pas changé.");
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : ta fonction "
                    "compare-t-elle les éléments avec cmp ?");
    } else if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("Quand tu déplaces un élément, copies-tu bien size octets ? Où gardes-tu "
                    "l'élément en cours pendant les décalages ?");
    } else if (cmp == t_cmp_int_desc) {
        tl_symptom("le tableau n'est pas dans l'ordre demandé par le comparateur.");
        tl_question("Ta fonction utilise-t-elle cmp pour comparer, ou compare-t-elle des "
                    "int elle-même ?");
    } else {
        tl_symptom_diff(in, expected, a, n);
        tl_question("Comment interprètes-tu le signe renvoyé par cmp ?");
    }
    tl_fail();
}

static const int SMALL[] = { 5, -3, 9, 0, 9, 2, -7, 4, 1, 8 };

/* Tri de struct student : on vérifie l'ordre des notes et la stabilité. */
static void check_students(const char *func, gsort_fn sort, const struct student *in,
                           size_t n)
{
    struct student *a = tl_malloc(n * sizeof *a);
    memcpy(a, in, n * sizeof *a);
    set_hints();
    sort(a, n, sizeof *a, t_cmp_grade);
    (void)func;
    for (size_t i = 0; i + 1 < n; i++) {
        bool order = a[i].grade <= a[i + 1].grade;
        bool stable = !(a[i].grade == a[i + 1].grade
                        && strcmp(a[i].name, a[i + 1].name) > 0);
        if (order && stable) {
            continue;
        }
        tl_line("%zu élèves triés par note (dans l'entrée, les noms de même note sont dans "
                "l'ordre alphabétique)", n);
        if (n <= 8) {
            printf("    obtenu  :");
            for (size_t j = 0; j < n; j++) {
                printf(" {%s, %d}", a[j].name, a[j].grade);
            }
            putchar('\n');
        }
        if (!order) {
            tl_symptom("%s (note %d) est placé avant %s (note %d).", a[i].name, a[i].grade,
                       a[i + 1].name, a[i + 1].grade);
            tl_question("Tes tests sur les int passent-ils ? Copies-tu des éléments entiers "
                        "de size octets ?");
        } else {
            tl_symptom("deux élèves de même note ont changé d'ordre (%s et %s, note %d).",
                       a[i + 1].name, a[i].name, a[i].grade);
            tl_question("Quand deux éléments sont équivalents (cmp renvoie 0), lequel passe "
                        "en premier ? Lequel ta fonction déplace-t-elle ?");
        }
        tl_fail();
    }
    tl_free(a);
}

static const struct student CLASS[] = {
    { "Lina", 12 }, { "Adam", 15 }, { "Omar", 12 }, { "Zoe", 9 }, { "Ines", 15 },
};

static void check_students_random(const char *func, gsort_fn sort, size_t n, uint64_t seed)
{
    struct student *in = tl_malloc(n * sizeof *in);
    tl_rand_seed(seed);
    for (size_t i = 0; i < n; i++) {
        snprintf(in[i].name, sizeof in[i].name, "e%06u", (unsigned)(i % 1000000));
        in[i].grade = tl_rand_range(0, 20);
    }
    check_students(func, sort, in, n);
    tl_free(in);
}

static void check_chars(const char *func, gsort_fn sort)
{
    char a[] = "trilab, tri en C";
    char expected[] = "trilab, tri en C";
    size_t n = strlen(a);
    set_hints();
    sort(a, n, 1, t_cmp_char);
    qsort(expected, n, 1, t_cmp_char);
    if (strcmp(a, expected) != 0) {
        tl_line("tri des caractères de \"trilab, tri en C\" (éléments de 1 octet)");
        tl_line("attendu : \"%s\"", expected);
        tl_line("obtenu  : \"%s\"", a);
        tl_symptom("les caractères ne sont pas triés.");
        tl_question("Avec size == 1, l'adresse de l'élément i vaut-elle bien base + i ?");
        tl_fail();
    }
    (void)func;
}

static void check_doubles(const char *func, gsort_fn sort)
{
    double a[] = { 0.5, -1.25, 3.0, 0.1, 0.30000000000000004, 0.3, -7.5 };
    double expected[] = { 0.5, -1.25, 3.0, 0.1, 0.30000000000000004, 0.3, -7.5 };
    size_t n = TL_LEN(a);
    set_hints();
    sort(a, n, sizeof(double), t_cmp_double);
    qsort(expected, n, sizeof(double), t_cmp_double);
    for (size_t i = 0; i < n; i++) {
        if (a[i] != expected[i]) {
            tl_line("tri de 7 double");
            tl_symptom("a[%zu] vaut %.17g au lieu de %.17g.", i, a[i], expected[i]);
            tl_question("Copies-tu les éléments avec size octets (8 pour un double), et pas "
                        "sizeof(int) ?");
            tl_fail();
        }
    }
    (void)func;
}

/* Gros éléments : une clé et 96 octets qui doivent la suivre. */
struct record {
    int key;
    unsigned char payload[96];
};

static int cmp_record(const void *x, const void *y)
{
    return t_cmp_int(&((const struct record *)x)->key, &((const struct record *)y)->key);
}

static void check_records(const char *func, gsort_fn sort)
{
    enum { N = 300 };
    struct record *r = tl_malloc(N * sizeof *r);
    tl_rand_seed(1616);
    for (size_t i = 0; i < N; i++) {
        r[i].key = tl_rand_range(-1000, 1000);
        memset(r[i].payload, r[i].key & 0xFF, sizeof r[i].payload);
    }
    set_hints();
    sort(r, N, sizeof *r, cmp_record);
    for (size_t i = 0; i < N; i++) {
        bool intact = true;
        for (size_t j = 0; j < sizeof r[i].payload; j++) {
            intact = intact && r[i].payload[j] == (unsigned char)(r[i].key & 0xFF);
        }
        if (!intact || (i + 1 < N && r[i].key > r[i + 1].key)) {
            tl_line("300 éléments de %zu octets", sizeof *r);
            if (!intact) {
                tl_symptom("l'élément %zu a une clé qui ne correspond plus à son contenu : "
                           "il a été copié en morceaux.", i);
                tl_question("Copies-tu les size octets d'un élément d'un seul tenant ?");
            } else {
                tl_symptom("les clés ne sont pas dans l'ordre (position %zu).", i);
                tl_question("Tes tests sur les int passent-ils ?");
            }
            tl_fail();
        }
    }
    tl_free(r);
    (void)func;
}

/* ---- Étape 5 ---- */

static void ins_vide(void)
{
    check_ints("insertion_sort_generic", insertion_sort_generic, NULL, 0, t_cmp_int,
               "croissant");
}

static void ins_int(void)
{
    check_ints("insertion_sort_generic", insertion_sort_generic, SMALL, 10, t_cmp_int,
               "croissant");
}

static void ins_int_decroissant(void)
{
    check_ints("insertion_sort_generic", insertion_sort_generic, SMALL, 10, t_cmp_int_desc,
               "décroissant (comparateur inversé)");
}

static void ins_char(void)
{
    check_chars("insertion_sort_generic", insertion_sort_generic);
}

static void ins_double(void)
{
    check_doubles("insertion_sort_generic", insertion_sort_generic);
}

static void ins_eleves(void)
{
    check_students("insertion_sort_generic", insertion_sort_generic, CLASS, TL_LEN(CLASS));
}

static void ins_eleves_nombreux(void)
{
    check_students_random("insertion_sort_generic", insertion_sort_generic, 2000, 1617);
}

static void ins_gros_elements(void)
{
    check_records("insertion_sort_generic", insertion_sort_generic);
}

static void ins_aleatoire(void)
{
    enum { N = 2000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1618);
    tl_fill_random(in, N, INT_MIN, INT_MAX);
    check_ints("insertion_sort_generic", insertion_sort_generic, in, N, t_cmp_int,
               "croissant");
    tl_ints_free(in);
}

/* ---- Étape 6 ---- */

static void fus_vide(void)
{
    check_ints("merge_sort_generic", merge_wrap, NULL, 0, t_cmp_int, "croissant");
}

static void fus_int(void)
{
    check_ints("merge_sort_generic", merge_wrap, SMALL, 10, t_cmp_int, "croissant");
}

static void fus_int_decroissant(void)
{
    check_ints("merge_sort_generic", merge_wrap, SMALL, 10, t_cmp_int_desc,
               "décroissant (comparateur inversé)");
}

static void fus_char(void)
{
    check_chars("merge_sort_generic", merge_wrap);
}

static void fus_double(void)
{
    check_doubles("merge_sort_generic", merge_wrap);
}

static void fus_eleves(void)
{
    check_students("merge_sort_generic", merge_wrap, CLASS, TL_LEN(CLASS));
}

static void fus_eleves_nombreux(void)
{
    check_students_random("merge_sort_generic", merge_wrap, 100000, 1619);
}

static void fus_gros_elements(void)
{
    check_records("merge_sort_generic", merge_wrap);
}

static void fus_grand(void)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(1620);
    tl_fill_random(in, N, INT_MIN, INT_MAX);
    check_ints("merge_sort_generic", merge_wrap, in, N, t_cmp_int, "croissant");
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "generic_swap", false },
    { 2, "cmp_int", false },
    { 3, "cmp_double", false },
    { 4, "cmp_student", false },
    { 5, "insertion_sort_generic", false },
    { 6, "merge_sort_generic", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, swap_int, "deux int"),
    TL_TEST(1, swap_double, "deux double"),
    TL_TEST(1, swap_student, "deux struct student"),
    TL_TEST(1, swap_char, "deux char"),
    TL_TEST(1, swap_gros_blocs, "deux blocs de 1000 octets"),
    TL_TEST(1, swap_meme_zone, "une zone avec elle-même"),
    TL_TEST(1, swap_zero_octet, "size = 0"),

    TL_TEST(2, ci_inferieur, "1 avant 2"),
    TL_TEST(2, ci_superieur, "2 après 1"),
    TL_TEST(2, ci_egal, "5 et 5"),
    TL_TEST(2, ci_extremes, "INT_MIN et INT_MAX"),

    TL_TEST(3, cd_ordre, "0.1 et 0.2"),
    TL_TEST(3, cd_egal, "1.5 et 1.5"),
    TL_TEST(3, cd_negatif, "-2.0 et 3.0"),
    TL_TEST(3, cd_petit_ecart, "0.3 et 0.30000000000000004"),

    TL_TEST(4, cs_notes, "notes différentes"),
    TL_TEST(4, cs_meme_note, "même note, noms différents"),
    TL_TEST(4, cs_extremes, "notes INT_MIN et INT_MAX"),

    TL_TEST(5, ins_vide, "n = 0 avec base = NULL"),
    TL_TEST(5, ins_int, "10 int"),
    TL_TEST(5, ins_int_decroissant, "10 int, comparateur inversé"),
    TL_TEST(5, ins_char, "des char (1 octet)"),
    TL_TEST(5, ins_double, "des double"),
    TL_TEST(5, ins_eleves, "5 élèves : stabilité"),
    TL_TEST(5, ins_eleves_nombreux, "2000 élèves : stabilité"),
    TL_TEST(5, ins_gros_elements, "éléments de 100 octets"),
    TL_TEST(5, ins_aleatoire, "2000 int aléatoires"),

    TL_TEST(6, fus_vide, "n = 0 avec base = NULL"),
    TL_TEST(6, fus_int, "10 int"),
    TL_TEST(6, fus_int_decroissant, "10 int, comparateur inversé"),
    TL_TEST(6, fus_char, "des char (1 octet)"),
    TL_TEST(6, fus_double, "des double"),
    TL_TEST(6, fus_eleves, "5 élèves : stabilité"),
    TL_TEST(6, fus_eleves_nombreux, "100 000 élèves : stabilité"),
    TL_TEST(6, fus_gros_elements, "éléments de 100 octets"),
    TL_TEST(6, fus_grand, "200 000 int aléatoires"),
};

TL_MAIN("16", "Tri générique façon qsort", STEPS, TESTS)

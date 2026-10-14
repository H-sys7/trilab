/* Exercice 07 — tests. NE PAS MODIFIER CE FICHIER. */
#include "mesure.h"
#include "tl_check.h"

#include <time.h>

/* Vrai si a[0..n) est une permutation de 0, 1, ..., n - 1. */
static bool is_identity_permutation(const int *a, size_t n)
{
    int *s = tl_ints_copy(a, n);
    qsort(s, n, sizeof(int), tl_cmp_int);
    bool ok = true;
    for (size_t i = 0; i < n; i++) {
        if (s[i] != (int)i) {
            ok = false;
            break;
        }
    }
    tl_ints_free(s);
    return ok;
}

/* ---------------- Étape 1 · fill_random ---------------- */

static void rnd_vide(void)
{
    tl_on_crash("Avec n == 0, ta fonction écrit-elle quand même une case ?");
    fill_random(NULL, 0, 1, 10);
}

static void rnd_bornes(void)
{
    enum { N = 1000, MAX = 9 };
    int *a = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        a[i] = -1;
    }
    tl_on_crash("Ta boucle écrit-elle en dehors de a[0..n) ?");
    fill_random(a, N, 7, MAX);
    tl_check_guards(a, N, "fill_random");
    size_t seen[MAX + 1] = { 0 };
    for (size_t i = 0; i < N; i++) {
        if (a[i] < 0 || a[i] > MAX) {
            tl_line("1000 valeurs tirées dans [0, %d]", MAX);
            tl_show("obtenu", a, N, i, NULL);
            if (a[i] == -1) {
                tl_symptom("la case a[%zu] n'a pas été remplie.", i);
                tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : ta "
                            "boucle remplit-elle toutes les cases ?");
            } else {
                tl_symptom("a[%zu] = %d est en dehors de [0, %d].", i, a[i], MAX);
                tl_question("rand() renvoie un entier de [0, RAND_MAX] : comment le "
                            "ramènes-tu dans [0, max] ?");
            }
            tl_fail();
        }
        seen[a[i]]++;
    }
    for (int v = 0; v <= MAX; v++) {
        if (seen[v] == 0) {
            tl_line("1000 valeurs tirées dans [0, %d]", MAX);
            tl_symptom("la valeur %d n'apparaît jamais sur 1000 tirages.", v);
            tl_question("Les deux bornes, 0 et max, peuvent-elles sortir ? Que donne "
                        "x %% max quand x est un multiple de max ?");
            tl_fail();
        }
    }
    tl_ints_free(a);
}

static void rnd_meme_graine(void)
{
    enum { N = 200 };
    int *a = tl_ints_new(N);
    int *b = tl_ints_new(N);
    fill_random(a, N, 123, 1000000);
    fill_random(b, N, 123, 1000000);
    if (!tl_ints_equal(a, b, N)) {
        size_t i = tl_first_diff(a, b, N);
        tl_line("deux appels avec la graine 123");
        tl_show("1er", a, N, i, NULL);
        tl_show("2e", b, N, i, NULL);
        tl_symptom("la même graine donne deux tableaux différents.");
        tl_question("Où ta fonction utilise-t-elle seed ?");
        tl_fail();
    }
    tl_ints_free(a);
    tl_ints_free(b);
}

static void rnd_graines_differentes(void)
{
    enum { N = 200 };
    int *a = tl_ints_new(N);
    int *b = tl_ints_new(N);
    fill_random(a, N, 1, 1000000);
    fill_random(b, N, 2, 1000000);
    if (tl_ints_equal(a, b, N)) {
        tl_line("un appel avec la graine 1, un autre avec la graine 2");
        tl_symptom("deux graines différentes donnent exactement le même tableau.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : ta fonction "
                    "transmet-elle seed au générateur avant de tirer ?");
        tl_fail();
    }
    tl_ints_free(a);
    tl_ints_free(b);
}

static void rnd_max_zero(void)
{
    enum { N = 50 };
    int *a = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        a[i] = -1;
    }
    tl_on_crash("Avec max == 0, ton calcul divise-t-il par zéro ?");
    fill_random(a, N, 5, 0);
    for (size_t i = 0; i < N; i++) {
        if (a[i] != 0) {
            tl_line("50 valeurs tirées dans [0, 0]");
            tl_show("obtenu", a, N, i, NULL);
            tl_symptom("a[%zu] = %d, alors que la seule valeur possible est 0.", i, a[i]);
            tl_question("Que donne ton calcul quand max vaut 0 ?");
            tl_fail();
        }
    }
    tl_ints_free(a);
}

/* ---------------- Étapes 2 et 3 · fill_sorted, fill_reversed ---------------- */

static void check_fill(const char *func, void (*fill)(int *, size_t), const int *expected,
                       size_t n)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta fonction écrit-elle quand même une case ? Que vaut "
                    "n - 1 quand n est un size_t égal à 0 ?");
        fill(NULL, 0);
        return;
    }
    int *a = tl_ints_new(n);
    for (size_t i = 0; i < n; i++) {
        a[i] = -7;
    }
    tl_on_crash("Ta boucle écrit-elle en dehors de a[0..n) ?");
    fill(a, n);
    tl_check_guards(a, n, func);
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        return;
    }
    size_t i = tl_first_diff(expected, a, n);
    tl_show("attendu", expected, n, i, NULL);
    tl_show("obtenu", a, n, i, NULL);
    if (a[i] == -7) {
        tl_symptom("la case a[%zu] n'a pas été remplie.", i);
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : ta boucle "
                    "passe-t-elle par toutes les cases ?");
    } else {
        tl_symptom("a[%zu] vaut %d au lieu de %d.", i, a[i], expected[i]);
        tl_question("Écris la valeur que ta fonction met dans les cases 0, 1 et n - 1 : "
                    "correspond-elle au contrat ?");
    }
    tl_fail();
}

static void sorted_vide(void)
{
    check_fill("fill_sorted", fill_sorted, NULL, 0);
}

static void sorted_cinq(void)
{
    static const int expected[] = { 0, 1, 2, 3, 4 };
    check_fill("fill_sorted", fill_sorted, expected, 5);
}

static void sorted_un(void)
{
    static const int expected[] = { 0 };
    check_fill("fill_sorted", fill_sorted, expected, 1);
}

static void reversed_vide(void)
{
    check_fill("fill_reversed", fill_reversed, NULL, 0);
}

static void reversed_cinq(void)
{
    static const int expected[] = { 4, 3, 2, 1, 0 };
    check_fill("fill_reversed", fill_reversed, expected, 5);
}

static void reversed_un(void)
{
    static const int expected[] = { 0 };
    check_fill("fill_reversed", fill_reversed, expected, 1);
}

/* ---------------- Étape 4 · fill_nearly_sorted ---------------- */

static void nearly_vide(void)
{
    tl_on_crash("Avec n == 0, ta fonction écrit-elle quand même une case ?");
    fill_nearly_sorted(NULL, 0, 1);
}

static void nearly_un(void)
{
    int *a = tl_ints_new(1);
    a[0] = -7;
    tl_on_crash("Avec n == 1, existe-t-il une paire de voisins à échanger ?");
    tl_on_timeout("Avec n == 1, combien d'échanges ta fonction cherche-t-elle à faire ?");
    fill_nearly_sorted(a, 1, 1);
    tl_check_guards(a, 1, "fill_nearly_sorted");
    if (a[0] != 0) {
        tl_line("n = 1");
        tl_line("attendu : {0}");
        tl_line("obtenu  : {%d}", a[0]);
        tl_symptom("avec un seul élément, le tableau doit valoir {0}.");
        tl_question("Si tu n'as pas commencé l'étape 4, c'est normal. Sinon : ta fonction "
                    "remplit-elle d'abord le tableau dans l'ordre ?");
        tl_fail();
    }
    tl_ints_free(a);
}

/* n = 2 avec 32 graines : une seule paire possible, j doit toujours valoir 0. */
static void nearly_deux(void)
{
    static const int expected[] = { 1, 0 };
    int *a = tl_ints_new(2);
    tl_on_crash("Pour 2 éléments, quelles valeurs de j sont possibles ? La case qui suit a[j] "
                "existe-t-elle toujours ?");
    for (unsigned seed = 1; seed <= 32; seed++) {
        fill_nearly_sorted(a, 2, seed);
        tl_check_guards(a, 2, "fill_nearly_sorted");
        if (!tl_ints_equal(a, expected, 2)) {
            tl_line("n = 2, graine %u : une seule paire de voisins, échangée une fois", seed);
            tl_report_ints(NULL, expected, a, 2, NULL, NULL);
            tl_symptom("pour 2 éléments, il faut exactement 1 échange : on attend {1, 0}.");
            tl_question("Combien d'échanges fais-tu quand n / 100 vaut 0 ?");
            tl_fail();
        }
    }
    tl_ints_free(a);
}

static void nearly_mille(void)
{
    enum { N = 1000, K = N / 100 };
    int *a = tl_ints_new(N);
    tl_on_crash("Quelles valeurs de j sont possibles ? La case qui suit a[j] existe-t-elle "
                "toujours ?");
    fill_nearly_sorted(a, N, 11);
    tl_check_guards(a, N, "fill_nearly_sorted");
    if (!is_identity_permutation(a, N)) {
        tl_line("n = 1000");
        tl_symptom("le tableau ne contient pas exactement les valeurs 0 à 999.");
        tl_question("Ton échange de voisins garde-t-il les deux valeurs ? Le tableau "
                    "est-il bien rempli avant les échanges ?");
        tl_fail();
    }
    size_t moved = 0;
    for (size_t i = 0; i < N; i++) {
        if (a[i] != (int)i) {
            moved++;
        }
        if (a[i] - (int)i > K || (int)i - a[i] > K) {
            tl_line("n = 1000 : 10 échanges de voisins");
            tl_symptom("la valeur %d se trouve en a[%zu], à plus de %d cases de sa place.",
                       a[i], i, K);
            tl_question("Tes échanges portent-ils bien sur deux voisins, et combien en "
                        "fais-tu ?");
            tl_fail();
        }
    }
    if (moved == 0 || moved > 2 * K) {
        tl_line("n = 1000 : 10 échanges de voisins");
        tl_symptom("%zu cases ne sont pas à leur place (attendu : entre 1 et %d).", moved,
                   2 * K);
        tl_question("Combien d'échanges ta fonction fait-elle pour n = 1000 ?");
        tl_fail();
    }
    tl_ints_free(a);
}

static void nearly_meme_graine(void)
{
    enum { N = 500 };
    int *a = tl_ints_new(N);
    int *b = tl_ints_new(N);
    fill_nearly_sorted(a, N, 99);
    fill_nearly_sorted(b, N, 99);
    if (!tl_ints_equal(a, b, N)) {
        tl_line("deux appels avec la graine 99");
        tl_symptom("la même graine donne deux tableaux différents.");
        tl_question("Où ta fonction utilise-t-elle seed ?");
        tl_fail();
    }
    tl_ints_free(a);
    tl_ints_free(b);
}

/* ---------------- Étape 5 · time_sort ---------------- */

static int spy_calls;
static int *spy_a;
static size_t spy_n;

/* « Tri » espion : enregistre son appel et occupe le processeur 50 ms. */
static void spy_sort_50ms(int *a, size_t n)
{
    spy_calls++;
    spy_a = a;
    spy_n = n;
    clock_t start = clock();
    while ((double)(clock() - start) < 0.05 * (double)CLOCKS_PER_SEC) {
        /* attente active */
    }
}

static void spy_sort_instant(int *a, size_t n)
{
    spy_calls++;
    spy_a = a;
    spy_n = n;
}

static void time_appel(void)
{
    int a[3] = { 3, 1, 2 };
    spy_calls = 0;
    (void)time_sort(spy_sort_instant, a, 3);
    if (spy_calls != 1 || spy_a != a || spy_n != 3) {
        tl_line("time_sort(tri, a, 3)");
        tl_line("appels du tri : %d (attendu : 1)", spy_calls);
        if (spy_calls == 0) {
            tl_symptom("le tri reçu en paramètre n'a jamais été appelé.");
            tl_question("Si tu n'as pas commencé l'étape 5, c'est normal. Sinon : comment "
                        "appelle-t-on une fonction reçue par un pointeur ?");
        } else if (spy_calls > 1) {
            tl_symptom("le tri a été appelé %d fois.", spy_calls);
            tl_question("Combien de fois le contrat demande-t-il d'appeler sort ?");
        } else {
            tl_symptom("le tri n'a pas reçu le tableau a et la taille n d'origine.");
            tl_question("Quels arguments passes-tu à sort ?");
        }
        tl_fail();
    }
}

static void time_50ms(void)
{
    int a[1] = { 0 };
    spy_calls = 0;
    double ms = time_sort(spy_sort_50ms, a, 1);
    if (ms >= 40.0 && ms <= 1000.0 && spy_calls == 1) {
        return;
    }
    tl_line("un tri qui dure 50 ms de calcul");
    tl_line("attendu : environ 50 (millisecondes)");
    tl_line("obtenu  : %g", ms);
    if (spy_calls != 1) {
        tl_symptom("le tri a été appelé %d fois au lieu d'une.", spy_calls);
        tl_question("Combien de fois le contrat demande-t-il d'appeler sort ?");
    } else if (ms == 0.0) {
        tl_symptom("la durée vaut exactement 0.");
        tl_question("Ton calcul de durée se fait-il en entiers ? Que donne 50000 / "
                    "1000000 dans un calcul en entiers ?");
    } else if (ms > 0.02 && ms < 0.2) {
        tl_symptom("la durée semble exprimée en secondes, pas en millisecondes.");
        tl_question("Combien de millisecondes y a-t-il dans une seconde ?");
    } else if (ms > 10000.0) {
        tl_symptom("la durée est beaucoup trop grande : ce sont peut-être des tops "
                   "d'horloge bruts.");
        tl_question("Que vaut CLOCKS_PER_SEC, et à quoi sert-il ?");
    } else {
        tl_symptom("la durée ne correspond pas à 50 ms.");
        tl_question("Prends-tu bien la mesure juste avant et juste après l'appel du tri ?");
    }
    tl_fail();
}

static void time_instant(void)
{
    int a[1] = { 0 };
    double ms = time_sort(spy_sort_instant, a, 1);
    if (ms < 0.0 || ms > 5.0) {
        tl_line("un tri qui ne fait rien");
        tl_line("attendu : entre 0 et 5 (millisecondes)");
        tl_line("obtenu  : %g", ms);
        tl_symptom("un appel quasi instantané ne devrait presque rien coûter.");
        tl_question("Mesures-tu seulement l'appel du tri, ou autre chose autour ?");
        tl_fail();
    }
}

/* ---------------- Étape 6 · insertion_sort_counted ---------------- */

static void check_counted(const int *in, size_t n, size_t exp_cmp, size_t exp_shift)
{
    struct op_stats st = { 999, 999 };
    int *a = n > 0 ? tl_ints_copy(in, n) : NULL;
    tl_on_crash("Que vaut ton indice quand l'élément à insérer est plus petit que tous les "
                "autres ?");
    insertion_sort_counted(a, n, &st);
    if (n > 0) {
        tl_check_guards(a, n, "insertion_sort_counted");
        int *expected = tl_ints_copy(in, n);
        qsort(expected, n, sizeof(int), tl_cmp_int);
        if (!tl_ints_equal(a, expected, n)) {
            tl_report_ints(in, expected, a, n, NULL, NULL);
            if (tl_ints_equal(a, in, n)) {
                tl_symptom("le tableau n'a pas été trié.");
                tl_question("Si tu n'as pas commencé l'étape 6, c'est normal. Sinon : "
                            "reprends ton tri par insertion de l'exercice 03.");
            } else {
                tl_symptom_diff(in, expected, a, n);
                tl_question("Ton tri marchait-il à l'exercice 03 ? Qu'as-tu changé en "
                            "ajoutant les compteurs ?");
            }
            tl_fail();
        }
        tl_ints_free(expected);
    }
    if (st.comparisons == exp_cmp && st.shifts == exp_shift) {
        tl_ints_free(a);
        return;
    }
    if (n > 0 && n <= TL_SHOW_MAX) {
        tl_show("entrée", in, n, 0, NULL);
    } else if (n == 0) {
        tl_line("entrée  : a = NULL, n = 0");
    }
    tl_line("attendu : %zu comparaisons, %zu décalages", exp_cmp, exp_shift);
    tl_line("obtenu  : %zu comparaisons, %zu décalages", st.comparisons, st.shifts);
    if (st.comparisons == exp_cmp + 999 || st.shifts == exp_shift + 999) {
        tl_symptom("les compteurs valaient 999 avant l'appel, et au moins l'un d'eux n'a "
                   "pas été remis à zéro.");
        tl_question("Que contient *stats au moment où ta fonction commence ?");
    } else if (st.comparisons == 0 && st.shifts == 0) {
        tl_symptom("aucune opération n'a été comptée.");
        tl_question("À quels endroits de ton tri faut-il augmenter chaque compteur ?");
    } else if (n > 0 && st.shifts == exp_shift && st.comparisons + n - 1 >= exp_cmp
               && st.comparisons < exp_cmp) {
        tl_symptom("les décalages sont justes, mais il manque des comparaisons.");
        tl_question("La comparaison qui arrête ta boucle, celle qui répond « non », "
                    "est-elle comptée ?");
    } else if (st.comparisons == exp_shift && st.shifts == exp_cmp) {
        tl_symptom("les deux compteurs semblent inversés.");
        tl_question("Quel compteur augmentes-tu à chaque comparaison, et lequel à chaque "
                    "décalage ?");
    } else {
        tl_symptom("les compteurs ne correspondent pas.");
        tl_question("Déroule ton tri à la main sur {3, 1, 2} en comptant chaque "
                    "comparaison et chaque décalage : trouves-tu 3 et 2 ?");
    }
    tl_fail();
}

static void cnt_vide(void)
{
    check_counted(NULL, 0, 0, 0);
}

static void cnt_un(void)
{
    static const int in[] = { 5 };
    check_counted(in, 1, 0, 0);
}

static void cnt_exemple(void)
{
    static const int in[] = { 3, 1, 2 };
    check_counted(in, 3, 3, 2);
}

static void cnt_trie(void)
{
    static const int in[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    check_counted(in, 10, 9, 0);
}

static void cnt_inverse(void)
{
    static const int in[] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    check_counted(in, 10, 45, 45);
}

static void cnt_egaux(void)
{
    static const int in[] = { 2, 2, 2, 2 };
    check_counted(in, 4, 3, 0);
}

/* 1000 valeurs : décalages = nombre d'inversions ; comparaisons entre décalages et
 * décalages + n - 1. */
static void cnt_aleatoire(void)
{
    enum { N = 1000 };
    int *in = tl_ints_new(N);
    tl_rand_seed(7007);
    tl_fill_random(in, N, -1000, 1000);
    size_t inversions = 0;
    for (size_t i = 0; i < N; i++) {
        for (size_t j = i + 1; j < N; j++) {
            if (in[i] > in[j]) {
                inversions++;
            }
        }
    }
    int *a = tl_ints_copy(in, N);
    struct op_stats st = { 0, 0 };
    insertion_sort_counted(a, N, &st);
    if (tl_first_unsorted(a, N) != N) {
        tl_line("1000 valeurs aléatoires");
        tl_symptom("le tableau n'est pas trié.");
        tl_question("Tes petits tests de l'étape 6 passent-ils ? Commence par eux.");
        tl_fail();
    }
    if (st.shifts != inversions || st.comparisons < st.shifts
        || st.comparisons > st.shifts + N - 1) {
        tl_line("1000 valeurs aléatoires");
        tl_line("attendu : %zu décalages, et entre %zu et %zu comparaisons", inversions,
                inversions, inversions + N - 1);
        tl_line("obtenu  : %zu décalages, %zu comparaisons", st.shifts, st.comparisons);
        tl_symptom("les compteurs ne correspondent pas à un tri par insertion.");
        tl_question("Chaque décalage déplace-t-il une valeur d'une seule case ? "
                    "Chaque comparaison est-elle comptée une seule fois ?");
        tl_fail();
    }
    tl_ints_free(a);
    tl_ints_free(in);
}

/* ---------------- Étape 7 · bench_write_csv ---------------- */

/* Lit tout le fichier path dans buf (taille size). Renvoie false s'il est absent. */
static bool read_file(const char *path, char *buf, size_t size)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        return false;
    }
    size_t len = fread(buf, 1, size - 1, f);
    buf[len] = '\0';
    fclose(f);
    return true;
}

/* Affiche un texte en rendant visibles les fins de ligne. */
static void show_text(const char *label, const char *text)
{
    printf("    %s", label);
    for (const char *p = text; *p != '\0'; p++) {
        if (*p == '\n') {
            fputs(p[1] != '\0' ? "\\n\n              " : "\\n", stdout);
        } else {
            putchar(*p);
        }
    }
    putchar('\n');
}

static void check_csv(const char *path, const struct bench_row *rows, size_t count,
                      const char *expected)
{
    char got[1024];
    remove(path);
    tl_on_crash("Ta fonction vérifie-t-elle que fopen a réussi avant d'écrire ?");
    int ret = bench_write_csv(path, rows, count);
    bool exists = read_file(path, got, sizeof got);
    remove(path);
    if (ret == 0 && exists && strcmp(got, expected) == 0) {
        return;
    }
    tl_line("fichier : %s, %zu ligne(s) de données", path, count);
    if (!exists) {
        tl_symptom("le fichier n'existe pas après l'appel (valeur renvoyée : %d).", ret);
        tl_question("Si tu n'as pas commencé l'étape 7, c'est normal. Sinon : avec quel "
                    "mode ouvres-tu le fichier ?");
        tl_fail();
    }
    show_text("attendu : ", expected);
    show_text("obtenu  : ", got);
    if (ret != 0) {
        tl_symptom("la fonction renvoie %d alors que le fichier a pu être écrit.", ret);
        tl_question("Que doit renvoyer ta fonction quand tout s'est bien passé ?");
    } else if (got[0] == '\0' || strlen(got) < strlen(expected) / 2) {
        tl_symptom("le fichier est vide ou incomplet après le retour de la fonction.");
        tl_question("Le texte écrit avec fprintf part-il vraiment dans le fichier avant "
                    "que ta fonction se termine ? Qu'est-ce qui le garantit ?");
    } else {
        size_t i = 0;
        while (got[i] == expected[i]) {
            i++;
        }
        tl_symptom("première différence au caractère %zu : attendu « %c », obtenu « %c ».",
                   i, expected[i] == '\n' ? '/' : expected[i], got[i] == '\n' ? '/' : got[i]);
        tl_question("Compare ton format de printf au contrat : séparateurs, nombre de "
                    "décimales, ratio vide quand il vaut 0, fin de ligne.");
    }
    tl_fail();
}

static void csv_entete(void)
{
    check_csv("build/test_csv_entete.csv", NULL, 0, "algo,profil,n,temps_ms,ratio\n");
}

static void csv_deux_lignes(void)
{
    static const struct bench_row rows[] = {
        { "bulle", "aleatoire", 1000, 0.8521, 0.0 },
        { "bulle", "aleatoire", 2000, 3.3, 3.872 },
    };
    check_csv("build/test_csv_lignes.csv", rows, 2,
              "algo,profil,n,temps_ms,ratio\n"
              "bulle,aleatoire,1000,0.852,\n"
              "bulle,aleatoire,2000,3.300,3.87\n");
}

static void csv_grand_n(void)
{
    static const struct bench_row rows[] = {
        { "shell", "presque_trie", 4194304, 1234.5678, 2.1 },
    };
    check_csv("build/test_csv_grand.csv", rows, 1,
              "algo,profil,n,temps_ms,ratio\n"
              "shell,presque_trie,4194304,1234.568,2.10\n");
}

static void csv_chemin_invalide(void)
{
    static const struct bench_row rows[] = {
        { "bulle", "trie", 1000, 1.0, 0.0 },
    };
    tl_on_crash("Que renvoie fopen quand le dossier n'existe pas, et que fais-tu dans ce "
                "cas ?");
    int ret = bench_write_csv("build/dossier_inexistant/x.csv", rows, 1);
    if (ret != -1) {
        tl_line("chemin : build/dossier_inexistant/x.csv");
        tl_line("attendu : -1");
        tl_line("obtenu  : %d", ret);
        tl_symptom("le fichier ne peut pas être créé, mais la fonction ne le signale pas.");
        tl_question("Comment sais-tu que fopen a échoué ?");
        tl_fail();
    }
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "fill_random", false },
    { 2, "fill_sorted", false },
    { 3, "fill_reversed", false },
    { 4, "fill_nearly_sorted", false },
    { 5, "time_sort", false },
    { 6, "insertion_sort_counted", false },
    { 7, "bench_write_csv", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, rnd_vide, "n = 0 avec a = NULL"),
    TL_TEST(1, rnd_bornes, "1000 valeurs dans [0, 9]"),
    TL_TEST(1, rnd_meme_graine, "même graine, même tableau"),
    TL_TEST(1, rnd_graines_differentes, "graines différentes, tableaux différents"),
    TL_TEST(1, rnd_max_zero, "max = 0"),

    TL_TEST(2, sorted_vide, "n = 0 avec a = NULL"),
    TL_TEST(2, sorted_un, "1 élément"),
    TL_TEST(2, sorted_cinq, "5 éléments"),

    TL_TEST(3, reversed_vide, "n = 0 avec a = NULL"),
    TL_TEST(3, reversed_un, "1 élément"),
    TL_TEST(3, reversed_cinq, "5 éléments"),

    TL_TEST(4, nearly_vide, "n = 0 avec a = NULL"),
    TL_TEST(4, nearly_un, "1 élément"),
    TL_TEST(4, nearly_deux, "2 éléments"),
    TL_TEST(4, nearly_mille, "1000 éléments"),
    TL_TEST(4, nearly_meme_graine, "même graine, même tableau"),

    TL_TEST(5, time_appel, "le tri est appelé une fois, avec a et n"),
    TL_TEST(5, time_50ms, "un tri de 50 ms"),
    TL_TEST(5, time_instant, "un tri instantané"),

    TL_TEST(6, cnt_vide, "n = 0 avec a = NULL"),
    TL_TEST(6, cnt_un, "1 élément"),
    TL_TEST(6, cnt_exemple, "{3, 1, 2}"),
    TL_TEST(6, cnt_trie, "10 éléments déjà triés"),
    TL_TEST(6, cnt_inverse, "10 éléments triés à l'envers"),
    TL_TEST(6, cnt_egaux, "4 éléments égaux"),
    TL_TEST(6, cnt_aleatoire, "1000 éléments aléatoires"),

    TL_TEST(7, csv_entete, "en-tête seul"),
    TL_TEST(7, csv_deux_lignes, "deux lignes, dont un ratio vide"),
    TL_TEST(7, csv_grand_n, "grand n, ratio 2.1"),
    TL_TEST(7, csv_chemin_invalide, "dossier inexistant"),
};

TL_MAIN("07", "Mesure et complexité", STEPS, TESTS)

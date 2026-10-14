/* TriLab — banc de mesure (make bench). NE PAS MODIFIER CE FICHIER.
 * Compilé par tools/bench.sh avec tes exercices validés (macros -DWITH_<NOM>).
 * Usage : bench --list                           liste des tris disponibles
 *         bench run <tri> <profil> <n>           temps médian en ms, puis nombre de mesures
 *                                                (code 5 : tri par comptage refusé)
 *         bench csv <lignes.txt> <sortie.csv>    export avec ton bench_write_csv
 */
#include "mesure.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WITH_BUBBLE
#include "bubble.h"
#endif
#ifdef WITH_SELECTION
#include "selection.h"
#endif
#ifdef WITH_INSERTION
#include "insertion.h"
#endif
#ifdef WITH_COCKTAIL
#include "cocktail.h"
#endif
#ifdef WITH_COMB
#include "comb.h"
#endif
#ifdef WITH_SHELL
#include "shell.h"
#endif
#ifdef WITH_MERGE
#include "merge.h"
#endif
#ifdef WITH_QUICK
#include "quick.h"
#endif
#ifdef WITH_BST
#include "bst.h"
#endif
#ifdef WITH_AVL
#include "avl.h"
#endif
#ifdef WITH_HEAP
#include "heap.h"
#endif
#ifdef WITH_COUNTING
#include "counting.h"
#endif
#ifdef WITH_RADIX
#include "radix.h"
#endif
#ifdef WITH_BUCKET
#include "bucket.h"
#endif
#ifdef WITH_GENERIC
#include "generic.h"
#endif
#ifdef WITH_INTRO
#include "intro.h"
#endif
#ifdef WITH_TIM
#include "tim.h"
#endif

#define BENCH_SEED 42u
#define BENCH_RUNS 5
#define BENCH_RUNS_SLOW 3      /* au-delà de BENCH_SLOW_MS, 3 mesures suffisent */
#define BENCH_SLOW_MS 2000.0
#define BENCH_VALUE_MAX 1000000000

static int bench_cmp_int(const void *x, const void *y)
{
    int a = *(const int *)x;
    int b = *(const int *)y;
    return (a > b) - (a < b);
}

/* Tri de référence : qsort de la bibliothèque standard. */
static void qsort_int(int *a, size_t n)
{
    qsort(a, n, sizeof *a, bench_cmp_int);
}

#ifdef WITH_COUNTING
/* Le comptage refuse une plage de valeurs trop grande : le banc le signale (code 5). */
static void counting_wrap(int *a, size_t n)
{
    if (counting_sort(a, n) != 0) {
        exit(5);
    }
}
#endif

#ifdef WITH_GENERIC
/* Le tri fusion générique, appliqué à des int : on mesure le coût de la généricité. */
static void merge_generic_wrap(int *a, size_t n)
{
    if (merge_sort_generic(a, n, sizeof *a, bench_cmp_int) != 0) {
        exit(4);
    }
}
#endif

typedef struct {
    const char *name;  /* nom court, utilisé dans le CSV */
    const char *label; /* nom affiché */
    sort_fn sort;
} bench_algo;

/* Registre des tris : seuls les exercices validés sont compilés. */
static const bench_algo ALGOS[] = {
#ifdef WITH_BUBBLE
    { "bulle", "Tri à bulles", bubble_sort },
#endif
#ifdef WITH_SELECTION
    { "selection", "Tri par sélection", selection_sort },
#endif
#ifdef WITH_INSERTION
    { "insertion", "Tri par insertion", insertion_sort },
#endif
#ifdef WITH_COCKTAIL
    { "cocktail", "Tri cocktail", cocktail_sort },
#endif
#ifdef WITH_COMB
    { "peigne", "Tri à peigne", comb_sort },
#endif
#ifdef WITH_SHELL
    { "shell", "Tri de Shell", shell_sort },
#endif
#ifdef WITH_MERGE
    { "fusion", "Tri fusion", merge_sort },
#endif
#ifdef WITH_QUICK
    { "rapide_naif", "Tri rapide naïf", quick_sort_naive },
    { "rapide", "Tri rapide (médiane de trois)", quick_sort },
#endif
#ifdef WITH_BST
    { "abr", "Tri par ABR", tree_sort },
#endif
#ifdef WITH_AVL
    { "avl", "Tri par AVL", avl_sort },
#endif
#ifdef WITH_HEAP
    { "tas", "Tri par tas", heap_sort },
#endif
#ifdef WITH_COUNTING
    { "comptage", "Tri par comptage", counting_wrap },
#endif
#ifdef WITH_RADIX
    { "radix", "Tri par base (radix LSD)", radix_sort },
#endif
#ifdef WITH_GENERIC
    { "fusion_generique", "Tri fusion générique (void *)", merge_generic_wrap },
#endif
#ifdef WITH_INTRO
    { "introsort", "Introsort", introsort },
#endif
#ifdef WITH_TIM
    { "timsort", "Timsort simplifié", timsort },
#endif
    { "qsort", "qsort de la libc (référence)", qsort_int },
};

#define ALGO_COUNT (sizeof ALGOS / sizeof ALGOS[0])

static const bench_algo *find_algo(const char *name)
{
    for (size_t i = 0; i < ALGO_COUNT; i++) {
        if (strcmp(ALGOS[i].name, name) == 0) {
            return &ALGOS[i];
        }
    }
    return NULL;
}

/* Remplit a selon le profil, avec les générateurs de l'exercice 07. */
static int fill_profile(int *a, size_t n, const char *profile)
{
    if (strcmp(profile, "aleatoire") == 0) {
        fill_random(a, n, BENCH_SEED, BENCH_VALUE_MAX);
    } else if (strcmp(profile, "trie") == 0) {
        fill_sorted(a, n);
    } else if (strcmp(profile, "inverse") == 0) {
        fill_reversed(a, n);
    } else if (strcmp(profile, "presque_trie") == 0) {
        fill_nearly_sorted(a, n, BENCH_SEED);
    } else {
        return -1;
    }
    return 0;
}

static int bench_cmp_double(const void *x, const void *y)
{
    double a = *(const double *)x;
    double b = *(const double *)y;
    return (a > b) - (a < b);
}

#ifdef WITH_BUCKET
/* Tri par seaux : son propre profil, des double tirés dans [0, 1). */
static int cmd_run_unit(const char *profile, size_t n)
{
    if (strcmp(profile, "unitaire") != 0) {
        fprintf(stderr, "bench run : le tri par seaux n'a que le profil « unitaire »\n");
        return 2;
    }
    int *ints = malloc(n * sizeof *ints);
    double *base = malloc(n * sizeof *base);
    double *work = malloc(n * sizeof *work);
    if (ints == NULL || base == NULL || work == NULL) {
        free(ints);
        free(base);
        free(work);
        return 4;
    }
    fill_random(ints, n, BENCH_SEED, BENCH_VALUE_MAX);
    for (size_t i = 0; i < n; i++) {
        base[i] = (double)ints[i] / ((double)BENCH_VALUE_MAX + 1.0);
    }
    free(ints);
    double times[BENCH_RUNS];
    int runs = BENCH_RUNS;
    int status = 0;
    for (int r = 0; r < runs && status == 0; r++) {
        memcpy(work, base, n * sizeof *work);
        clock_t start = clock();
        int ret = bucket_sort(work, n);
        clock_t end = clock();
        times[r] = (double)(end - start) * 1000.0 / (double)CLOCKS_PER_SEC;
        if (ret != 0) {
            status = 4;
        }
        for (size_t i = 1; i < n && status == 0; i++) {
            if (work[i - 1] > work[i]) {
                printf("INCORRECT %zu\n", i - 1);
                status = 3;
            }
        }
        if (r == 0 && times[0] > BENCH_SLOW_MS) {
            runs = BENCH_RUNS_SLOW;
        }
    }
    if (status == 0) {
        qsort(times, (size_t)runs, sizeof times[0], bench_cmp_double);
        printf("%.3f %d\n", times[runs / 2], runs);
    }
    free(base);
    free(work);
    return status;
}
#endif

/* Une mesure : médiane de plusieurs exécutions sur les mêmes données. */
static int cmd_run(const char *name, const char *profile, const char *n_text)
{
    const bench_algo *algo = find_algo(name);
    char *end = NULL;
    unsigned long long nn = strtoull(n_text, &end, 10);
    if (*end != '\0' || nn == 0) {
        fprintf(stderr, "bench run : taille inconnue\n");
        return 2;
    }
    size_t n = (size_t)nn;
#ifdef WITH_BUCKET
    if (strcmp(name, "seaux") == 0) {
        return cmd_run_unit(profile, n);
    }
#endif
    if (algo == NULL) {
        fprintf(stderr, "bench run : tri inconnu « %s »\n", name);
        return 2;
    }
    int *base = malloc(n * sizeof *base);
    int *work = malloc(n * sizeof *work);
    if (base == NULL || work == NULL) {
        fprintf(stderr, "bench run : mémoire insuffisante pour n = %zu\n", n);
        free(base);
        free(work);
        return 4;
    }
    if (fill_profile(base, n, profile) != 0) {
        fprintf(stderr, "bench run : profil inconnu « %s »\n", profile);
        free(base);
        free(work);
        return 2;
    }
    double times[BENCH_RUNS];
    int runs = BENCH_RUNS;
    for (int r = 0; r < runs; r++) {
        memcpy(work, base, n * sizeof *work);
        times[r] = time_sort(algo->sort, work, n);
        for (size_t i = 1; i < n; i++) {
            if (work[i - 1] > work[i]) {
                printf("INCORRECT %zu\n", i - 1);
                free(base);
                free(work);
                return 3;
            }
        }
        if (r == 0 && times[0] > BENCH_SLOW_MS) {
            runs = BENCH_RUNS_SLOW;
        }
    }
    qsort(times, (size_t)runs, sizeof times[0], bench_cmp_double);
    printf("%.3f %d\n", times[runs / 2], runs);
    free(base);
    free(work);
    return 0;
}

/* Recopie une chaîne dans un bloc alloué (strdup n'est pas du C11). */
static char *copy_text(const char *s)
{
    size_t len = strlen(s) + 1;
    char *c = malloc(len);
    if (c != NULL) {
        memcpy(c, s, len);
    }
    return c;
}

/* Lit les lignes « tri profil n temps ratio » et les exporte avec bench_write_csv. */
static int cmd_csv(const char *in_path, const char *out_path)
{
    FILE *in = fopen(in_path, "r");
    if (in == NULL) {
        fprintf(stderr, "bench csv : impossible de lire %s\n", in_path);
        return 1;
    }
    struct bench_row *rows = NULL;
    size_t count = 0;
    size_t cap = 0;
    char algo[64];
    char profile[64];
    size_t n;
    double t;
    double ratio;
    int status = 0;
    while (fscanf(in, "%63s %63s %zu %lf %lf", algo, profile, &n, &t, &ratio) == 5) {
        if (count == cap) {
            size_t new_cap = cap ? 2 * cap : 64;
            struct bench_row *grown = realloc(rows, new_cap * sizeof *rows);
            if (grown == NULL) {
                status = 4;
                break;
            }
            rows = grown;
            cap = new_cap;
        }
        rows[count].algo = copy_text(algo);
        rows[count].profile = copy_text(profile);
        rows[count].n = n;
        rows[count].time_ms = t;
        rows[count].ratio = ratio;
        count++;
    }
    fclose(in);
    if (status == 0 && bench_write_csv(out_path, rows, count) != 0) {
        fprintf(stderr, "bench csv : bench_write_csv n'a pas pu écrire %s\n", out_path);
        status = 1;
    }
    for (size_t i = 0; i < count; i++) {
        free((char *)rows[i].algo);
        free((char *)rows[i].profile);
    }
    free(rows);
    return status;
}

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--list") == 0) {
        for (size_t i = 0; i < ALGO_COUNT; i++) {
            printf("%s\t%s\taleatoire,trie,inverse,presque_trie\n", ALGOS[i].name,
                   ALGOS[i].label);
        }
#ifdef WITH_BUCKET
        printf("seaux\tTri par seaux\tunitaire\n");
#endif
        return 0;
    }
    if (argc == 5 && strcmp(argv[1], "run") == 0) {
        return cmd_run(argv[2], argv[3], argv[4]);
    }
    if (argc == 4 && strcmp(argv[1], "csv") == 0) {
        return cmd_csv(argv[2], argv[3]);
    }
    fprintf(stderr, "Usage : bench --list | bench run <tri> <profil> <n> | "
                    "bench csv <lignes> <sortie.csv>\n");
    return 2;
}

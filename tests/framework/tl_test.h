/* TriLab — mini-framework de test. NE PAS MODIFIER CE FICHIER.
 *
 * Un binaire de test comprend :
 *   --title   numéro et titre de l'exercice
 *   --steps   liste des étapes (numéro, bonus, fonction)
 *   --list    liste des tests (identifiant, étape, timeout, description)
 *   <id>      lance un seul test : code 0 si réussi, 1 si échec (message sur la sortie standard)
 * Le lanceur tools/run_tests.sh lance chaque test dans son propre processus.
 */
#ifndef TL_TEST_H
#define TL_TEST_H

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void (*tl_fn)(void);

typedef struct {
    int number;       /* numéro d'étape : 1, 2, ... */
    const char *name; /* fonction travaillée à cette étape */
    bool bonus;       /* étape lancée seulement avec BONUS=1 */
} tl_step;

typedef struct {
    const char *id;    /* identifiant unique, pour make debug T=<id> */
    int step;          /* étape à laquelle appartient le test */
    const char *label; /* description du cas, en français */
    tl_fn fn;
    int timeout;       /* en secondes ; 0 = valeur par défaut du lanceur */
} tl_test;

#define TL_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))

/* Déclare une entrée de la table des tests. */
#define TL_TEST(step, fn, label) { #fn, (step), (label), (fn), 0 }
#define TL_TEST_T(step, fn, label, timeout) { #fn, (step), (label), (fn), (timeout) }

/* ------------------------------------------------------------------------
 * Allocations du test : gardées dans un registre global, libérées en cas
 * d'échec. Les fuites signalées par ASan ou Valgrind sont donc celles de
 * l'apprenant, jamais celles du test.
 * ------------------------------------------------------------------------ */

#define TL_MAX_ALLOCS 64

static void *tl_allocs[TL_MAX_ALLOCS];

static inline void tl_release_all(void)
{
    for (size_t i = 0; i < TL_MAX_ALLOCS; i++) {
        free(tl_allocs[i]);
        tl_allocs[i] = NULL;
    }
}

static inline void *tl_malloc(size_t bytes)
{
    void *p = malloc(bytes > 0 ? bytes : 1);
    if (p == NULL) {
        printf("    Erreur interne du test : mémoire insuffisante.\n");
        tl_release_all();
        exit(2);
    }
    for (size_t i = 0; i < TL_MAX_ALLOCS; i++) {
        if (tl_allocs[i] == NULL) {
            tl_allocs[i] = p;
            return p;
        }
    }
    printf("    Erreur interne du test : trop d'allocations.\n");
    free(p);
    tl_release_all();
    exit(2);
}

static inline void tl_free(void *p)
{
    if (p == NULL) {
        return;
    }
    for (size_t i = 0; i < TL_MAX_ALLOCS; i++) {
        if (tl_allocs[i] == p) {
            tl_allocs[i] = NULL;
            break;
        }
    }
    free(p);
}

/* ------------------------------------------------------------------------
 * Messages d'échec : symptôme puis question, jamais la correction.
 * ------------------------------------------------------------------------ */

#define TL_WRAP 74

/* Nombre de caractères affichés (UTF-8) dans s[0..len). */
static inline size_t tl_width(const char *s, size_t len)
{
    size_t w = 0;
    for (size_t i = 0; i < len; i++) {
        if (((unsigned char)s[i] & 0xC0u) != 0x80u) {
            w++;
        }
    }
    return w;
}

/* Affiche « prefix + texte » en coupant les lignes trop longues. */
static inline void tl_print_wrapped(const char *prefix, const char *text)
{
    size_t indent = tl_width(prefix, strlen(prefix));
    size_t col = indent;
    const char *p = text;
    bool glue = false; /* le mot précédent était « : le suivant reste sur la même ligne */
    fputs(prefix, stdout);
    while (*p != '\0') {
        while (*p == ' ') {
            p++;
        }
        size_t len = strcspn(p, " ");
        if (len == 0) {
            break;
        }
        size_t w = tl_width(p, len);
        /* Ponctuation et » ne commencent jamais une ligne. */
        bool punct = (len == 1 && strchr("?!:;", *p) != NULL) || strncmp(p, "»", 2) == 0;
        if (col > indent && col + 1 + w > TL_WRAP && !punct && !glue) {
            printf("\n%*s", (int)indent, "");
            col = indent;
        } else if (col > indent) {
            putchar(' ');
            col++;
        }
        fwrite(p, 1, len, stdout);
        glue = len == 2 && strncmp(p, "«", 2) == 0;
        col += w;
        p += len;
    }
    putchar('\n');
}

static inline void tl_vprint_wrapped(const char *prefix, const char *fmt, va_list ap)
{
    char buf[1024];
    vsnprintf(buf, sizeof buf, fmt, ap);
    tl_print_wrapped(prefix, buf);
}

/* Ligne libre, indentée, affichée telle quelle. */
static inline void tl_line(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("    ", stdout);
    vprintf(fmt, ap);
    putchar('\n');
    va_end(ap);
}

/* « Symptôme : ... » — ce que l'oracle a observé. */
static inline void tl_symptom(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    tl_vprint_wrapped("    Symptôme : ", fmt, ap);
    va_end(ap);
}

/* « Question : ... » — oriente vers la cause, sans donner la correction. */
static inline void tl_question(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    tl_vprint_wrapped("    Question : ", fmt, ap);
    va_end(ap);
}

/* Question affichée par le lanceur si le test plante (à appeler avant l'appel testé). */
static inline void tl_on_crash(const char *question)
{
    printf("@@plantage %s\n", question);
}

/* Question affichée par le lanceur si UBSan signale un comportement indéfini. */
static inline void tl_on_ub(const char *question)
{
    printf("@@indefini %s\n", question);
}

/* Question affichée par le lanceur si le test dépasse son temps. */
static inline void tl_on_timeout(const char *question)
{
    printf("@@temps %s\n", question);
}

/* Termine le test en échec. */
static inline _Noreturn void tl_fail(void)
{
    tl_release_all();
    exit(1);
}

/* ------------------------------------------------------------------------
 * Affichage des tableaux : 20 éléments au plus, sinon une fenêtre autour
 * de l'indice « focus » (typiquement le premier écart).
 * ------------------------------------------------------------------------ */

#define TL_SHOW_MAX 20
#define TL_WINDOW 4

static inline void tl_print_ints(const int *a, size_t n, size_t focus)
{
    if (a == NULL) {
        fputs("NULL", stdout);
        return;
    }
    size_t from = 0;
    size_t to = n;
    if (n > TL_SHOW_MAX) {
        if (focus >= n) {
            focus = 0;
        }
        from = focus > TL_WINDOW ? focus - TL_WINDOW : 0;
        to = focus + TL_WINDOW + 1 < n ? focus + TL_WINDOW + 1 : n;
    }
    fputs(from > 0 ? "{… " : "{", stdout);
    for (size_t i = from; i < to; i++) {
        printf(i > from ? ", %d" : "%d", a[i]);
    }
    fputs(to < n ? " …}" : "}", stdout);
    if (n > TL_SHOW_MAX) {
        printf(" (indices %zu à %zu sur %zu)", from, to - 1, n);
    }
}

/* Affiche « label : {..}   note ». note peut valoir NULL. */
static inline void tl_show(const char *label, const int *a, size_t n, size_t focus,
                           const char *note)
{
    size_t w = tl_width(label, strlen(label));
    printf("    %s%*s: ", label, (int)(w < 8 ? 8 - w : 0), "");
    tl_print_ints(a, n, focus);
    if (note != NULL) {
        printf("   %s", note);
    }
    putchar('\n');
}

/* Écrit {a0, a1, ...} dans buf (pour les descriptions). */
static inline const char *tl_fmt_ints(char *buf, size_t size, const int *a, size_t n)
{
    size_t used = 0;
    int w = snprintf(buf, size, "{");
    used = w > 0 ? (size_t)w : 0;
    for (size_t i = 0; i < n && used < size; i++) {
        w = snprintf(buf + used, size - used, i > 0 ? ", %d" : "%d", a[i]);
        used += w > 0 ? (size_t)w : 0;
    }
    if (used < size) {
        snprintf(buf + used, size - used, "}");
    }
    return buf;
}

/* ------------------------------------------------------------------------
 * Tableaux d'entiers protégés. En mode base, une sentinelle entoure le
 * tableau pour repérer une écriture hors bornes. En mode check, pas de
 * sentinelle : AddressSanitizer voit lui-même le moindre débordement.
 * ------------------------------------------------------------------------ */

#if defined(__SANITIZE_ADDRESS__)
#define TL_GUARDS 0
#else
#define TL_GUARDS 1
#endif

#define TL_GUARD_LEFT INT_MAX
#define TL_GUARD_RIGHT INT_MIN

/* Alloue un tableau de n entiers (contenu indéterminé). */
static inline int *tl_ints_new(size_t n)
{
    int *base = tl_malloc((n + 2 * TL_GUARDS) * sizeof(int));
#if TL_GUARDS
    base[0] = TL_GUARD_LEFT;
    base[n + 1] = TL_GUARD_RIGHT;
#endif
    return base + TL_GUARDS;
}

/* Alloue une copie de src[0..n). */
static inline int *tl_ints_copy(const int *src, size_t n)
{
    int *a = tl_ints_new(n);
    if (n > 0) {
        memcpy(a, src, n * sizeof(int));
    }
    return a;
}

static inline void tl_ints_free(int *a)
{
    if (a != NULL) {
        tl_free(a - TL_GUARDS);
    }
}

/* Vérifie les sentinelles après un appel à func. Échec si elles ont changé. */
static inline void tl_check_guards(const int *a, size_t n, const char *func)
{
#if TL_GUARDS
    if (a[-1] != TL_GUARD_LEFT) {
        tl_symptom("%s a écrit juste avant le début du tableau (dans la case a[-1]).", func);
        tl_question("Quel est le plus petit indice valide, et ton indice peut-il "
                    "descendre plus bas ?");
        tl_fail();
    }
    if (a[n] != TL_GUARD_RIGHT) {
        tl_symptom("%s a écrit juste après la fin du tableau (dans la case a[%zu]).", func, n);
        tl_question("Avec %zu élément%s, quel est le dernier indice valide ? Ta boucle "
                    "va-t-elle plus loin ?", n, n > 1 ? "s" : "");
        tl_fail();
    }
#else
    (void)a;
    (void)n;
    (void)func;
#endif
}

/* ------------------------------------------------------------------------
 * Programme principal.
 * ------------------------------------------------------------------------ */

static inline int tl_main(int argc, char **argv, const char *num, const char *title,
                          const tl_step *steps, size_t nsteps,
                          const tl_test *tests, size_t ntests)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc != 2) {
        printf("Exercice %s · %s\n", num, title);
        printf("Lance les tests avec make test, ou un seul test avec make debug T=<test>.\n");
        return 2;
    }
    const char *arg = argv[1];
    if (strcmp(arg, "--title") == 0) {
        printf("%s\t%s\n", num, title);
        return 0;
    }
    if (strcmp(arg, "--steps") == 0) {
        for (size_t i = 0; i < nsteps; i++) {
            printf("%d\t%d\t%s\n", steps[i].number, steps[i].bonus ? 1 : 0, steps[i].name);
        }
        return 0;
    }
    if (strcmp(arg, "--list") == 0) {
        for (size_t i = 0; i < ntests; i++) {
            printf("%s\t%d\t%d\t%s\n", tests[i].id, tests[i].step, tests[i].timeout,
                   tests[i].label);
        }
        return 0;
    }
    for (size_t i = 0; i < ntests; i++) {
        if (strcmp(arg, tests[i].id) == 0) {
            tests[i].fn();
            tl_release_all();
            return 0;
        }
    }
    printf("Test inconnu : %s. La liste : make debug sans T.\n", arg);
    return 2;
}

/* Définit main() à partir des tables des étapes et des tests. */
#define TL_MAIN(num, title, steps, tests)                                              \
    int main(int argc, char **argv)                                                     \
    {                                                                                  \
        return tl_main(argc, argv, (num), (title), (steps), TL_LEN(steps), (tests),     \
                       TL_LEN(tests));                                                 \
    }

#endif

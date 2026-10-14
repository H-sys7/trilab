/* TriLab — oracles : transforment un résultat faux en symptôme précis. NE PAS MODIFIER CE FICHIER.
 * Ils vérifient des propriétés (trié, permutation, stable, tas, ABR, AVL) sans jamais
 * montrer comment obtenir le résultat.
 */
#ifndef TL_CHECK_H
#define TL_CHECK_H

#include "tl_rand.h"
#include "tl_test.h"

/* ------------------------------------------------------------------------
 * Oracles sur les tableaux d'entiers.
 * ------------------------------------------------------------------------ */

static inline int tl_cmp_int(const void *x, const void *y)
{
    int a = *(const int *)x;
    int b = *(const int *)y;
    return (a > b) - (a < b);
}

/* Premier indice i tel que a[i] > a[i + 1], ou n si a[0..n) est trié. */
static inline size_t tl_first_unsorted(const int *a, size_t n)
{
    for (size_t i = 0; i + 1 < n; i++) {
        if (a[i] > a[i + 1]) {
            return i;
        }
    }
    return n;
}

/* Premier indice où a et b diffèrent, ou n s'ils sont identiques. */
static inline size_t tl_first_diff(const int *a, const int *b, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return i;
        }
    }
    return n;
}

static inline bool tl_ints_equal(const int *a, const int *b, size_t n)
{
    return tl_first_diff(a, b, n) == n;
}

/* Résultat de l'oracle « permutation ». */
typedef struct {
    bool ok;            /* out contient exactement les valeurs de in */
    bool has_lost;      /* une valeur apparaît moins souvent dans out */
    int lost;
    size_t lost_in, lost_out;
    bool has_extra;     /* une valeur apparaît plus souvent dans out */
    int extra;
    size_t extra_in, extra_out;
} tl_perm;

/* Compare les multiensembles de in[0..n) et out[0..n). */
static inline tl_perm tl_check_permutation(const int *in, const int *out, size_t n)
{
    tl_perm r = { true, false, 0, 0, 0, false, 0, 0, 0 };
    if (n == 0) {
        return r;
    }
    int *x = tl_malloc(n * sizeof(int));
    int *y = tl_malloc(n * sizeof(int));
    memcpy(x, in, n * sizeof(int));
    memcpy(y, out, n * sizeof(int));
    qsort(x, n, sizeof(int), tl_cmp_int);
    qsort(y, n, sizeof(int), tl_cmp_int);
    size_t i = 0;
    size_t j = 0;
    while ((i < n || j < n) && !(r.has_lost && r.has_extra)) {
        int v = (j >= n || (i < n && x[i] <= y[j])) ? x[i] : y[j];
        size_t ci = 0;
        size_t cj = 0;
        while (i < n && x[i] == v) {
            i++;
            ci++;
        }
        while (j < n && y[j] == v) {
            j++;
            cj++;
        }
        if (ci > cj && !r.has_lost) {
            r.has_lost = true;
            r.lost = v;
            r.lost_in = ci;
            r.lost_out = cj;
        } else if (cj > ci && !r.has_extra) {
            r.has_extra = true;
            r.extra = v;
            r.extra_in = ci;
            r.extra_out = cj;
        }
    }
    r.ok = !r.has_lost && !r.has_extra;
    tl_free(x);
    tl_free(y);
    return r;
}

/* Affiche le symptôme d'une permutation ratée (valeur perdue, dupliquée). */
static inline void tl_symptom_permutation(tl_perm p)
{
    char lost[160] = "";
    char extra[160] = "";
    if (p.has_extra) {
        snprintf(extra, sizeof extra, "%d apparaît %zu fois au lieu de %zu", p.extra,
                 p.extra_out, p.extra_in);
    }
    if (p.has_lost) {
        if (p.lost_out == 0) {
            snprintf(lost, sizeof lost, "%d a disparu", p.lost);
        } else {
            snprintf(lost, sizeof lost, "%d apparaît %zu fois au lieu de %zu", p.lost,
                     p.lost_out, p.lost_in);
        }
    }
    tl_symptom("des valeurs ont été perdues ou dupliquées : %s%s%s.", extra,
               (p.has_extra && p.has_lost) ? ", et " : "", lost);
}

/* Affiche entrée / attendu / obtenu, centrés sur le premier écart. */
static inline void tl_report_ints(const int *input, const int *expected, const int *got,
                                  size_t n, const char *note_expected, const char *note_got)
{
    size_t focus = tl_first_diff(expected, got, n);
    if (input != NULL && n <= TL_SHOW_MAX) {
        tl_show("entrée", input, n, focus, NULL);
    }
    tl_show("attendu", expected, n, focus, note_expected);
    tl_show("obtenu", got, n, focus, note_got);
}

/* Symptôme générique quand got diffère de expected : perte de valeurs, ou premier écart. */
static inline void tl_symptom_diff(const int *input, const int *expected, const int *got,
                                   size_t n)
{
    tl_perm p = tl_check_permutation(input != NULL ? input : expected, got, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        return;
    }
    size_t i = tl_first_diff(expected, got, n);
    tl_symptom("toutes les valeurs sont là, mais la première différence est à l'indice %zu "
               "(attendu %d, obtenu %d).", i, expected[i], got[i]);
}

/* ------------------------------------------------------------------------
 * Vérification complète d'un tri d'entiers.
 * ------------------------------------------------------------------------ */

typedef void (*tl_sort_int_fn)(int *, size_t);

/* Trie une copie de input avec sort, puis compare au résultat de qsort.
 * question : posée si toutes les valeurs sont là mais mal rangées (NULL = question par défaut). */
static inline void tl_check_sort_int(const char *func, tl_sort_int_fn sort, const int *input,
                                     size_t n, const char *question)
{
    if (n == 0) {
        tl_on_crash("Avec n == 0, ta fonction lit-elle ou écrit-elle quand même une case ?");
        sort(NULL, 0);
        return;
    }
    int *a = tl_ints_copy(input, n);
    int *expected = tl_ints_copy(input, n);
    qsort(expected, n, sizeof(int), tl_cmp_int);

    sort(a, n);
    tl_check_guards(a, n, func);
    if (tl_ints_equal(a, expected, n)) {
        tl_ints_free(a);
        tl_ints_free(expected);
        return;
    }

    tl_report_ints(input, expected, a, n, NULL, NULL);
    if (tl_ints_equal(a, input, n)) {
        tl_symptom("le tableau n'a pas changé du tout.");
        tl_question("Si tu n'as pas encore commencé cette étape, c'est normal. Sinon : "
                    "pour %zu éléments, combien de tours fait la boucle principale de %s ?",
                    n, func);
        tl_fail();
    }
    tl_perm p = tl_check_permutation(input, a, n);
    if (!p.ok) {
        tl_symptom_permutation(p);
        tl_question("À quel moment une valeur peut-elle être écrasée avant d'avoir été "
                    "mise à l'abri ailleurs ?");
        tl_fail();
    }
    size_t i = tl_first_unsorted(a, n);
    tl_symptom("toutes les valeurs sont là, mais a[%zu] = %d est placé avant a[%zu] = %d.",
               i, a[i], i + 1, a[i + 1]);
    tl_question("%s", question != NULL ? question
                                       : "Ton tri s'arrête-t-il avant que tout le tableau "
                                         "soit en ordre ?");
    tl_fail();
}

/* Trie n entiers aléatoires de [lo, hi] (graine fixe). */
static inline void tl_check_sort_random(const char *func, tl_sort_int_fn sort, size_t n,
                                        uint64_t seed, int lo, int hi)
{
    int *in = tl_ints_new(n);
    tl_rand_seed(seed);
    tl_fill_random(in, n, lo, hi);
    tl_check_sort_int(func, sort, in, n, NULL);
    tl_ints_free(in);
}

/* ------------------------------------------------------------------------
 * Suite standard des 7 cas pour chaque tri d'entiers (section 8).
 * TL_SORT_SUITE_DEFINE(f, grand_n) définit les fonctions de test,
 * TL_SORT_SUITE_ENTRIES(étape, f) les ajoute à la table.
 * ------------------------------------------------------------------------ */

#define TL_SORT_SUITE_DEFINE(f, big_n)                                                  \
    static void f##_vide(void)                                                          \
    {                                                                                   \
        tl_check_sort_int(#f, f, NULL, 0, NULL);                                        \
    }                                                                                   \
    static void f##_un(void)                                                            \
    {                                                                                   \
        static const int in[] = { 42 };                                                 \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_deux_ordre(void)                                                    \
    {                                                                                   \
        static const int in[] = { 1, 2 };                                               \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_deux_inverse(void)                                                  \
    {                                                                                   \
        static const int in[] = { 2, 1 };                                               \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_deja_trie(void)                                                     \
    {                                                                                   \
        static const int in[] = { -3, 0, 2, 5, 8, 13, 21, 34, 55, 89 };                 \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_inverse(void)                                                       \
    {                                                                                   \
        static const int in[] = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 };                       \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_doublons(void)                                                      \
    {                                                                                   \
        static const int in[] = { 5, 3, 5, 1, 3, 5, 0, 1 };                             \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_egaux(void)                                                         \
    {                                                                                   \
        static const int in[] = { 7, 7, 7, 7, 7, 7 };                                   \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_extremes(void)                                                      \
    {                                                                                   \
        static const int in[] = { 0, INT_MAX, -7, INT_MIN, 3, -1, INT_MAX, INT_MIN };   \
        tl_check_sort_int(#f, f, in, TL_LEN(in), NULL);                                 \
    }                                                                                   \
    static void f##_aleatoire(void)                                                     \
    {                                                                                   \
        tl_check_sort_random(#f, f, 1000, 1001, -1000000, 1000000);                     \
    }                                                                                   \
    static void f##_grand(void)                                                         \
    {                                                                                   \
        tl_on_timeout("Si toutes tes boucles s'arrêtent, ton tri fait-il trop de "      \
                      "travail ? Combien de passes fait-il sur n éléments ?");          \
        tl_check_sort_random(#f, f, (big_n), 2002, INT_MIN, INT_MAX);                   \
    }

#define TL_SORT_SUITE_ENTRIES(step, f)                                                  \
    TL_TEST(step, f##_vide, "n = 0 avec a = NULL"),                                     \
    TL_TEST(step, f##_un, "1 élément"),                                                 \
    TL_TEST(step, f##_deux_ordre, "2 éléments dans l'ordre"),                           \
    TL_TEST(step, f##_deux_inverse, "2 éléments inversés"),                             \
    TL_TEST(step, f##_deja_trie, "déjà trié"),                                          \
    TL_TEST(step, f##_inverse, "trié à l'envers"),                                      \
    TL_TEST(step, f##_doublons, "doublons"),                                            \
    TL_TEST(step, f##_egaux, "tous égaux"),                                             \
    TL_TEST(step, f##_extremes, "négatifs, INT_MIN et INT_MAX"),                        \
    TL_TEST(step, f##_aleatoire, "aléatoire, n = 1000, comparé à qsort"),              \
    TL_TEST(step, f##_grand, "grand tableau aléatoire, comparé à qsort")

/* ------------------------------------------------------------------------
 * Stabilité : chaque élément porte sa clé et son indice d'origine.
 * ------------------------------------------------------------------------ */

typedef struct {
    int key;
    size_t origin;
} tl_keyed;

/* Premier indice i où deux clés égales ont changé d'ordre (a[i] et a[i + 1]), ou n. */
static inline size_t tl_first_unstable(const tl_keyed *a, size_t n)
{
    for (size_t i = 0; i + 1 < n; i++) {
        if (a[i].key == a[i + 1].key && a[i].origin > a[i + 1].origin) {
            return i;
        }
    }
    return n;
}

/* ------------------------------------------------------------------------
 * Tas max rangé dans un tableau : parent (i - 1) / 2, enfants 2i + 1 et 2i + 2.
 * ------------------------------------------------------------------------ */

/* Premier indice i > 0 dont la valeur dépasse celle de son parent, ou n. */
static inline size_t tl_first_heap_violation(const int *a, size_t n)
{
    for (size_t i = 1; i < n; i++) {
        if (a[(i - 1) / 2] < a[i]) {
            return i;
        }
    }
    return n;
}

/* ------------------------------------------------------------------------
 * Arbres : l'oracle ne connaît pas la struct de l'exercice, il passe par
 * trois fonctions d'accès fournies par le test.
 * ------------------------------------------------------------------------ */

typedef struct {
    const void *(*left)(const void *node);
    const void *(*right)(const void *node);
    int (*key)(const void *node);
    int (*height)(const void *node); /* hauteur stockée dans le nœud, ou NULL */
} tl_tree_ops;

/* Ajoute du texte formaté à buf[*used..size). */
static inline void tl_append(char *buf, size_t size, size_t *used, const char *fmt, ...)
{
    if (*used >= size) {
        return;
    }
    va_list ap;
    va_start(ap, fmt);
    int w = vsnprintf(buf + *used, size - *used, fmt, ap);
    va_end(ap);
    if (w > 0) {
        *used += (size_t)w;
    }
}

static inline void tl_tree_format_in(const void *node, const tl_tree_ops *ops, char *buf,
                                     size_t size, size_t *used, int depth)
{
    if (node == NULL) {
        tl_append(buf, size, used, ".");
        return;
    }
    if (depth > 64) {
        tl_append(buf, size, used, "…");
        return;
    }
    tl_append(buf, size, used, "%d", ops->key(node));
    if (ops->height != NULL) {
        tl_append(buf, size, used, "[%d]", ops->height(node));
    }
    const void *l = ops->left(node);
    const void *r = ops->right(node);
    if (l != NULL || r != NULL) {
        tl_append(buf, size, used, "(");
        tl_tree_format_in(l, ops, buf, size, used, depth + 1);
        tl_append(buf, size, used, ",");
        tl_tree_format_in(r, ops, buf, size, used, depth + 1);
        tl_append(buf, size, used, ")");
    }
}

/* Écrit l'arbre en texte : « clé », ou « clé(gauche,droite) » avec « . » pour un enfant
 * absent ; si ops->height existe, chaque clé est suivie de [hauteur stockée].
 * Exemples : 5(3,8(.,9))   20[2](10[1],30[1])   . (arbre vide) */
static inline const char *tl_tree_format(const void *root, const tl_tree_ops *ops, char *buf,
                                         size_t size)
{
    size_t used = 0;
    buf[0] = '\0';
    tl_tree_format_in(root, ops, buf, size, &used, 0);
    return buf;
}

/* Nombre de nœuds. */
static inline size_t tl_tree_count(const void *node, const tl_tree_ops *ops)
{
    if (node == NULL) {
        return 0;
    }
    return 1 + tl_tree_count(ops->left(node), ops) + tl_tree_count(ops->right(node), ops);
}

/* Clés dans l'ordre gauche, nœud, droite : en écrit au plus max dans out. Renvoie le total. */
static inline size_t tl_tree_keys(const void *node, const tl_tree_ops *ops, int *out, size_t max,
                                  size_t done)
{
    if (node == NULL) {
        return done;
    }
    done = tl_tree_keys(ops->left(node), ops, out, max, done);
    if (done < max) {
        out[done] = ops->key(node);
    }
    done = tl_tree_keys(ops->right(node), ops, out, max, done + 1);
    return done;
}

/* Vérifie chaque hauteur stockée (ops->height) contre la hauteur réelle. Renvoie la hauteur
 * réelle ; *ok passe à false au premier nœud faux, dont la clé va dans *bad. */
static inline int tl_tree_check_heights(const void *node, const tl_tree_ops *ops, bool *ok,
                                        int *bad)
{
    if (node == NULL) {
        return 0;
    }
    int hl = tl_tree_check_heights(ops->left(node), ops, ok, bad);
    int hr = tl_tree_check_heights(ops->right(node), ops, ok, bad);
    int h = 1 + (hl > hr ? hl : hr);
    if (*ok && ops->height(node) != h) {
        *ok = false;
        *bad = ops->key(node);
    }
    return h;
}

/* ABR avec égaux à droite : gauche < clé <= droite. bad reçoit la clé fautive. */
static inline bool tl_tree_is_bst_in(const void *node, const tl_tree_ops *ops, long long lo,
                                     long long hi, int *bad)
{
    if (node == NULL) {
        return true;
    }
    int k = ops->key(node);
    if ((long long)k < lo || (long long)k >= hi) {
        *bad = k;
        return false;
    }
    return tl_tree_is_bst_in(ops->left(node), ops, lo, k, bad)
           && tl_tree_is_bst_in(ops->right(node), ops, k, hi, bad);
}

static inline bool tl_tree_is_bst(const void *root, const tl_tree_ops *ops, int *bad)
{
    return tl_tree_is_bst_in(root, ops, LLONG_MIN, LLONG_MAX, bad);
}

/* Hauteur réelle (arbre vide : 0, feuille : 1). */
static inline int tl_tree_height(const void *node, const tl_tree_ops *ops)
{
    if (node == NULL) {
        return 0;
    }
    int hl = tl_tree_height(ops->left(node), ops);
    int hr = tl_tree_height(ops->right(node), ops);
    return 1 + (hl > hr ? hl : hr);
}

/* Équilibre AVL : en chaque nœud, les hauteurs des deux sous-arbres diffèrent d'au plus 1.
 * Renvoie la hauteur, ou -1 si déséquilibré (bad reçoit la clé du nœud fautif). */
static inline int tl_tree_avl_height(const void *node, const tl_tree_ops *ops, int *bad)
{
    if (node == NULL) {
        return 0;
    }
    int hl = tl_tree_avl_height(ops->left(node), ops, bad);
    if (hl < 0) {
        return -1;
    }
    int hr = tl_tree_avl_height(ops->right(node), ops, bad);
    if (hr < 0) {
        return -1;
    }
    if (hl - hr > 1 || hr - hl > 1) {
        *bad = ops->key(node);
        return -1;
    }
    return 1 + (hl > hr ? hl : hr);
}

static inline bool tl_tree_is_avl(const void *root, const tl_tree_ops *ops, int *bad)
{
    return tl_tree_avl_height(root, ops, bad) >= 0;
}

#endif

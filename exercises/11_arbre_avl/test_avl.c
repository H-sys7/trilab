/* Exercice 11 — tests. NE PAS MODIFIER CE FICHIER. */
#include "avl.h"
#include "tl_check.h"

/* Accès aux nœuds pour les oracles de tl_check.h. */
static const void *node_left(const void *n)
{
    return ((const struct avl_node *)n)->left;
}

static const void *node_right(const void *n)
{
    return ((const struct avl_node *)n)->right;
}

static int node_key(const void *n)
{
    return ((const struct avl_node *)n)->key;
}

static int node_height(const void *n)
{
    return ((const struct avl_node *)n)->height;
}

static const tl_tree_ops OPS = { node_left, node_right, node_key, node_height };
static const tl_tree_ops OPS_SHAPE = { node_left, node_right, node_key, NULL };

/* Les arbres construits par les tests restent accessibles jusqu'à la fin du programme :
 * ils ne comptent pas comme des fuites, et le test n'a pas à les libérer. */
static struct avl_node *kept_tree;

#define NOTATION "(notation : clé[hauteur](sous-arbre gauche,sous-arbre droit), « . » = vide)"

/* ---------------- Étape 1 · avl_height ---------------- */

static void check_height(const struct avl_node *node, const char *what, int expected)
{
    tl_on_crash("Ta fonction teste-t-elle si node vaut NULL avant de lire son champ ?");
    int got = avl_height(node);
    if (got == expected) {
        return;
    }
    tl_line("nœud    : %s", what);
    tl_line("attendu : %d", expected);
    tl_line("obtenu  : %d", got);
    if (node == NULL) {
        tl_symptom("un arbre vide doit avoir une hauteur de 0.");
        tl_question("Que renvoie ta fonction quand node vaut NULL ?");
    } else {
        tl_symptom("la fonction ne renvoie pas la hauteur stockée dans le nœud.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : quel champ "
                    "de la struct contient la hauteur ?");
    }
    tl_fail();
}

static void h_vide(void)
{
    check_height(NULL, "NULL", 0);
}

static void h_feuille(void)
{
    static const struct avl_node leaf = { 4, 1, NULL, NULL };
    check_height(&leaf, "une feuille de hauteur 1", 1);
}

static void h_stockee(void)
{
    static const struct avl_node node = { 4, 7, NULL, NULL };
    check_height(&node, "un nœud dont le champ height vaut 7", 7);
}

/* ---------------- Étapes 2 et 3 · rotations ---------------- */

typedef struct avl_node *(*rotate_fn)(struct avl_node *);

static void check_rotation(const char *func, rotate_fn rotate, struct avl_node *root,
                           const char *before, const char *expected)
{
    char got[256];
    char shape_got[256];
    char shape_exp[256];
    tl_on_crash("Lis-tu un champ d'un pointeur qui vaut NULL ? Quel enfant est garanti non "
                "NULL ?");
    struct avl_node *r = rotate(root);
    tl_tree_format(r, &OPS, got, sizeof got);
    if (strcmp(got, expected) == 0) {
        return;
    }
    tl_line("avant   : %s", before);
    tl_line("attendu : %s", expected);
    tl_line("obtenu  : %s", r == NULL ? "NULL" : got);
    tl_line(NOTATION);
    /* Même forme sans les hauteurs ? Alors seules les hauteurs sont fausses. */
    size_t used = 0;
    shape_exp[0] = '\0';
    for (const char *p = expected; *p != '\0'; p++) {
        if (*p == '[') {
            while (*p != ']') {
                p++;
            }
            continue;
        }
        tl_append(shape_exp, sizeof shape_exp, &used, "%c", *p);
    }
    tl_tree_format(r, &OPS_SHAPE, shape_got, sizeof shape_got);
    if (r == root && strcmp(got, before) == 0) {
        tl_symptom("%s renvoie l'ancienne racine, et rien n'a bougé.", func);
        tl_question("Si tu n'as pas commencé cette étape, c'est normal. Sinon : quel nœud "
                    "devient la racine après la rotation ?");
    } else if (strcmp(shape_got, shape_exp) == 0) {
        tl_symptom("la forme est juste, mais une hauteur stockée est fausse.");
        tl_question("Dans quel ordre mets-tu à jour les hauteurs : le nœud qui descend "
                    "d'abord, ou celui qui monte ? Te sers-tu de avl_height pour les "
                    "enfants ?");
    } else if (r == root) {
        tl_symptom("%s renvoie l'ancienne racine au lieu de la nouvelle.", func);
        tl_question("Quelle valeur ta fonction renvoie-t-elle ?");
    } else {
        tl_symptom("les nœuds ne sont pas aux bonnes places.");
        tl_question("Le sous-arbre du milieu (entre les deux nœuds qui tournent) change de "
                    "parent : où doit-il aller ?");
    }
    tl_fail();
}

static void rotg_simple(void)
{
    static struct avl_node y = { 20, 1, NULL, NULL };
    static struct avl_node x = { 10, 2, NULL, &y };
    check_rotation("avl_rotate_left", avl_rotate_left, &x, "10[2](.,20[1])", "20[2](10[1],.)");
}

static void rotg_complete(void)
{
    static struct avl_node a = { 5, 1, NULL, NULL };
    static struct avl_node b = { 15, 1, NULL, NULL };
    static struct avl_node c = { 30, 1, NULL, NULL };
    static struct avl_node y = { 20, 2, &b, &c };
    static struct avl_node x = { 10, 3, &a, &y };
    check_rotation("avl_rotate_left", avl_rotate_left, &x, "10[3](5[1],20[2](15[1],30[1]))",
                   "20[3](10[2](5[1],15[1]),30[1])");
}

/* Les hauteurs viennent des champs height des enfants, pas d'un nouveau parcours. */
static void rotg_hauteurs_stockees(void)
{
    static struct avl_node a = { 5, 2, NULL, NULL };
    static struct avl_node b = { 15, 2, NULL, NULL };
    static struct avl_node c = { 30, 1, NULL, NULL };
    static struct avl_node y = { 20, 3, &b, &c };
    static struct avl_node x = { 10, 4, &a, &y };
    check_rotation("avl_rotate_left", avl_rotate_left, &x, "10[4](5[2],20[3](15[2],30[1]))",
                   "20[4](10[3](5[2],15[2]),30[1])");
}

static void rotd_simple(void)
{
    static struct avl_node x = { 10, 1, NULL, NULL };
    static struct avl_node y = { 20, 2, &x, NULL };
    check_rotation("avl_rotate_right", avl_rotate_right, &y, "20[2](10[1],.)",
                   "10[2](.,20[1])");
}

static void rotd_complete(void)
{
    static struct avl_node a = { 5, 1, NULL, NULL };
    static struct avl_node b = { 15, 1, NULL, NULL };
    static struct avl_node c = { 30, 1, NULL, NULL };
    static struct avl_node x = { 10, 2, &a, &b };
    static struct avl_node y = { 20, 3, &x, &c };
    check_rotation("avl_rotate_right", avl_rotate_right, &y, "20[3](10[2](5[1],15[1]),30[1])",
                   "10[3](5[1],20[2](15[1],30[1]))");
}

static void rotd_hauteurs_stockees(void)
{
    static struct avl_node a = { 5, 1, NULL, NULL };
    static struct avl_node b = { 15, 2, NULL, NULL };
    static struct avl_node c = { 30, 2, NULL, NULL };
    static struct avl_node x = { 10, 3, &a, &b };
    static struct avl_node y = { 20, 4, &x, &c };
    check_rotation("avl_rotate_right", avl_rotate_right, &y, "20[4](10[3](5[1],15[2]),30[2])",
                   "10[4](5[1],20[3](15[2],30[2]))");
}

/* ---------------- Étape 4 · avl_insert ---------------- */

/* Vérifie les invariants d'un AVL construit par insertions. keys : clés insérées. */
static void check_avl_invariants(const struct avl_node *root, const int *keys, size_t k,
                                 const char *what)
{
    size_t count = tl_tree_count(root, &OPS);
    if (count != k) {
        tl_line("%s", what);
        if (root == NULL) {
            tl_symptom("l'arbre est vide.");
            tl_question("Si tu n'as pas commencé l'étape 4, c'est normal. Sinon : ta "
                        "fonction renvoie-t-elle la nouvelle racine dans tous les cas ?");
        } else {
            tl_symptom("l'arbre contient %zu nœud(s) au lieu de %zu.", count, k);
            tl_question("Le nouveau nœud est-il accroché à son parent ? Utilises-tu la "
                        "valeur renvoyée par l'appel récursif ?");
        }
        tl_fail();
    }
    int *inorder = tl_ints_new(k > 0 ? k : 1);
    int *sorted = tl_ints_copy(keys, k > 0 ? k : 1);
    tl_tree_keys(root, &OPS, inorder, k, 0);
    qsort(sorted, k, sizeof(int), tl_cmp_int);
    if (!tl_ints_equal(inorder, sorted, k)) {
        tl_line("%s", what);
        size_t i = tl_first_diff(sorted, inorder, k);
        tl_symptom("les clés lues dans l'ordre infixe ne sont pas triées : position %zu, "
                   "attendu %d, obtenu %d.", i, sorted[i], inorder[i]);
        tl_question("Tes rotations gardent-elles l'ordre infixe ? Le nouveau nœud va-t-il "
                    "du bon côté ?");
        tl_fail();
    }
    bool ok = true;
    int bad = 0;
    tl_tree_check_heights(root, &OPS, &ok, &bad);
    if (!ok) {
        tl_line("%s", what);
        tl_symptom("le nœud de clé %d stocke une hauteur fausse.", bad);
        tl_question("Après une insertion et après chaque rotation, mets-tu à jour la "
                    "hauteur de chaque nœud du chemin ?");
        tl_fail();
    }
    if (!tl_tree_is_avl(root, &OPS, &bad)) {
        tl_line("%s", what);
        tl_symptom("le nœud de clé %d est déséquilibré : ses deux sous-arbres diffèrent de "
                   "plus de 1 en hauteur.", bad);
        tl_question("Au retour de l'insertion, vérifies-tu l'équilibre de chaque nœud du "
                    "chemin, pas seulement de la racine ?");
        tl_fail();
    }
    tl_ints_free(inorder);
    tl_ints_free(sorted);
}

static void check_inserts(const int *keys, size_t k, const char *expected)
{
    struct avl_node *root = NULL;
    char seq[128];
    char got[512];
    char what[192];
    tl_fmt_ints(seq, sizeof seq, keys, k);
    snprintf(what, sizeof what, "insertions : %s, dans un arbre vide", seq);
    tl_on_crash("Les champs du nouveau nœud (left, right, height) sont-ils tous "
                "initialisés ? Lis-tu un champ d'un pointeur NULL pendant une rotation ?");
    for (size_t i = 0; i < k; i++) {
        root = avl_insert(root, keys[i]);
        kept_tree = root;
    }
    tl_tree_format(root, &OPS, got, sizeof got);
    if (strcmp(got, expected) == 0) {
        return;
    }
    check_avl_invariants(root, keys, k, what);
    tl_line("%s", what);
    tl_line("attendu : %s", expected);
    tl_line("obtenu  : %s", got);
    tl_line(NOTATION);
    tl_symptom("l'arbre est un AVL valide, mais pas celui qu'on attend.");
    tl_question("Quel cas de déséquilibre est-ce (gauche-gauche, gauche-droite, droite-droite, "
                "droite-gauche), et quelle(s) rotation(s) lui correspond(ent) ?");
    tl_fail();
}

static void avli_vide(void)
{
    static const int k[] = { 10 };
    check_inserts(k, 1, "10[1]");
}

static void avli_sans_rotation(void)
{
    static const int k[] = { 20, 10, 30, 5 };
    check_inserts(k, 4, "20[3](10[2](5[1],.),30[1])");
}

static void avli_droite_droite(void)
{
    static const int k[] = { 10, 20, 30 };
    check_inserts(k, 3, "20[2](10[1],30[1])");
}

static void avli_gauche_gauche(void)
{
    static const int k[] = { 30, 20, 10 };
    check_inserts(k, 3, "20[2](10[1],30[1])");
}

static void avli_droite_gauche(void)
{
    static const int k[] = { 10, 30, 20 };
    check_inserts(k, 3, "20[2](10[1],30[1])");
}

static void avli_gauche_droite(void)
{
    static const int k[] = { 30, 10, 20 };
    check_inserts(k, 3, "20[2](10[1],30[1])");
}

static void avli_sept(void)
{
    static const int k[] = { 1, 2, 3, 4, 5, 6, 7 };
    check_inserts(k, 7, "4[3](2[2](1[1],3[1]),6[2](5[1],7[1]))");
}

static void avli_egaux(void)
{
    static const int k[] = { 5, 5, 5 };
    check_inserts(k, 3, "5[2](5[1],5[1])");
}

static void avli_aleatoire(void)
{
    enum { N = 2000 };
    int *keys = tl_ints_new(N);
    tl_rand_seed(1111);
    tl_fill_random(keys, N, -300, 300);
    struct avl_node *root = NULL;
    for (size_t i = 0; i < N; i++) {
        root = avl_insert(root, keys[i]);
        kept_tree = root;
    }
    check_avl_invariants(root, keys, N, "2000 insertions de valeurs entre -300 et 300");
    tl_ints_free(keys);
}

static void avli_croissant_grand(void)
{
    enum { N = 100000 };
    int *keys = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        keys[i] = (int)i;
    }
    tl_on_timeout("Ton arbre reste-t-il équilibré ? Tes rotations sont-elles appliquées ?");
    struct avl_node *root = NULL;
    for (size_t i = 0; i < N; i++) {
        root = avl_insert(root, keys[i]);
        kept_tree = root;
    }
    int h = avl_height(root);
    if (h > 24) {
        tl_line("100 000 insertions de 0, 1, 2, ...");
        tl_symptom("la hauteur stockée de la racine vaut %d (au plus 24 attendu).", h);
        tl_question("Au retour de l'insertion, rééquilibres-tu chaque nœud du chemin ?");
        tl_fail();
    }
    check_avl_invariants(root, keys, N, "100 000 insertions de 0, 1, 2, ...");
    tl_ints_free(keys);
}

/* ---------------- Étape 5 · avl_sort ---------------- */

TL_SORT_SUITE_DEFINE(avl_sort, 200000)

static void sort_deja_trie_grand(void)
{
    enum { N = 200000 };
    int *in = tl_ints_new(N);
    for (size_t i = 0; i < N; i++) {
        in[i] = (int)i;
    }
    tl_on_timeout("Ton tri utilise-t-il avl_insert, qui garde l'arbre équilibré ?");
    tl_check_sort_int("avl_sort", avl_sort, in, N, NULL);
    tl_ints_free(in);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "avl_height", false },
    { 2, "avl_rotate_left", false },
    { 3, "avl_rotate_right", false },
    { 4, "avl_insert", false },
    { 5, "avl_sort", false },
};

static const tl_test TESTS[] = {
    TL_TEST(1, h_vide, "arbre vide"),
    TL_TEST(1, h_feuille, "une feuille"),
    TL_TEST(1, h_stockee, "hauteur stockée 7"),

    TL_TEST(2, rotg_simple, "10(.,20) tourne à gauche"),
    TL_TEST(2, rotg_complete, "10(5,20(15,30)) tourne à gauche"),
    TL_TEST(2, rotg_hauteurs_stockees, "hauteurs tirées des champs des enfants"),

    TL_TEST(3, rotd_simple, "20(10,.) tourne à droite"),
    TL_TEST(3, rotd_complete, "20(10(5,15),30) tourne à droite"),
    TL_TEST(3, rotd_hauteurs_stockees, "hauteurs tirées des champs des enfants"),

    TL_TEST(4, avli_vide, "insérer 10 dans un arbre vide"),
    TL_TEST(4, avli_sans_rotation, "insérer 20, 10, 30, 5 (sans rotation)"),
    TL_TEST(4, avli_droite_droite, "insérer 10, 20, 30 (droite-droite)"),
    TL_TEST(4, avli_gauche_gauche, "insérer 30, 20, 10 (gauche-gauche)"),
    TL_TEST(4, avli_droite_gauche, "insérer 10, 30, 20 (droite-gauche)"),
    TL_TEST(4, avli_gauche_droite, "insérer 30, 10, 20 (gauche-droite)"),
    TL_TEST(4, avli_sept, "insérer 1 à 7 dans l'ordre"),
    TL_TEST(4, avli_egaux, "insérer 5, 5, 5"),
    TL_TEST(4, avli_aleatoire, "2000 insertions"),
    TL_TEST(4, avli_croissant_grand, "100 000 insertions croissantes"),

    TL_SORT_SUITE_ENTRIES(5, avl_sort),
    TL_TEST(5, sort_deja_trie_grand, "200 000 éléments déjà triés"),
};

TL_MAIN("11", "Tri par arbre AVL", STEPS, TESTS)

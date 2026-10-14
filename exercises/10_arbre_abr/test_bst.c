/* Exercice 10 — tests. NE PAS MODIFIER CE FICHIER. */
#include "bst.h"
#include "tl_check.h"

/* Accès aux nœuds pour les oracles de tl_check.h. */
static const void *node_left(const void *n)
{
    return ((const struct bst_node *)n)->left;
}

static const void *node_right(const void *n)
{
    return ((const struct bst_node *)n)->right;
}

static int node_key(const void *n)
{
    return ((const struct bst_node *)n)->key;
}

static const tl_tree_ops OPS = { node_left, node_right, node_key, NULL };

/* Les arbres construits par les tests des étapes 1 et 2 restent accessibles jusqu'à la fin
 * du programme : ils ne comptent pas comme des fuites, et le test n'a pas à les libérer. */
static struct bst_node *kept_trees[4];

static void keep(struct bst_node *root)
{
    for (size_t i = 0; i < TL_LEN(kept_trees); i++) {
        if (kept_trees[i] == NULL) {
            kept_trees[i] = root;
            return;
        }
    }
}

typedef int (*insert_fn)(struct bst_node **, int);

/* ---------------- Étape 1 · bst_insert ---------------- */

static void check_inserts(const char *func, insert_fn insert, const int *keys, size_t k,
                          const char *expected)
{
    struct bst_node *root = NULL;
    char seq[256];
    char got[512];
    tl_fmt_ints(seq, sizeof seq, keys, k);
    tl_on_crash("Les champs left et right du nouveau nœud sont-ils initialisés avant qu'on "
                "les lise ?");
    for (size_t i = 0; i < k; i++) {
        int ret = insert(&root, keys[i]);
        if (ret != 0) {
            keep(root);
            tl_line("insertions : %s", seq);
            tl_symptom("%s renvoie %d à l'insertion de %d, au lieu de 0.", func, ret, keys[i]);
            tl_question("Que doit renvoyer ta fonction quand l'allocation a réussi ?");
            tl_fail();
        }
    }
    keep(root);
    tl_tree_format(root, &OPS, got, sizeof got);
    if (strcmp(got, expected) == 0) {
        return;
    }
    tl_line("insertions : %s, dans un arbre vide", seq);
    tl_line("attendu : %s", expected);
    tl_line("obtenu  : %s", root == NULL ? "(arbre vide)" : got);
    tl_line("(notation : clé(sous-arbre gauche,sous-arbre droit), « . » = pas d'enfant)");
    int bad = 0;
    size_t count = tl_tree_count(root, &OPS);
    if (root == NULL) {
        tl_symptom("l'arbre est resté vide : la racine vaut toujours NULL.");
        tl_question("Si tu n'as pas commencé l'étape 1, c'est normal. Sinon : ta fonction "
                    "modifie-t-elle *root, ou seulement sa copie locale de root ?");
    } else if (count != k) {
        tl_symptom("l'arbre contient %zu nœud(s) au lieu de %zu.", count, k);
        tl_question("Le nouveau nœud est-il bien accroché à son parent ? Que modifies-tu : "
                    "le pointeur de l'arbre, ou une copie locale ?");
    } else if (!tl_tree_is_bst(root, &OPS, &bad)) {
        tl_symptom("la clé %d n'est pas du bon côté d'un de ses ancêtres.", bad);
        tl_question("Quand la nouvelle clé est plus petite, de quel côté vas-tu ? Et quand "
                    "elle est égale ?");
    } else {
        tl_symptom("l'arbre est un ABR valide, mais pas celui qu'on obtient en insérant "
                   "chaque clé comme une feuille, dans l'ordre.");
        tl_question("Déroule les insertions à la main : où doit arriver chaque clé ?");
    }
    tl_fail();
}

static void ins_vide(void)
{
    static const int k[] = { 7 };
    check_inserts("bst_insert", bst_insert, k, 1, "7");
}

static void ins_trois(void)
{
    static const int k[] = { 5, 3, 8 };
    check_inserts("bst_insert", bst_insert, k, 3, "5(3,8)");
}

static void ins_exemple(void)
{
    static const int k[] = { 5, 3, 8, 1, 4, 9, 7 };
    check_inserts("bst_insert", bst_insert, k, 7, "5(3(1,4),8(7,9))");
}

static void ins_egaux(void)
{
    static const int k[] = { 5, 5, 3, 5 };
    check_inserts("bst_insert", bst_insert, k, 4, "5(3,5(.,5))");
}

static void ins_croissant(void)
{
    static const int k[] = { 1, 2, 3, 4 };
    check_inserts("bst_insert", bst_insert, k, 4, "1(.,2(.,3(.,4)))");
}

static void ins_decroissant(void)
{
    static const int k[] = { 4, 3, 2, 1 };
    check_inserts("bst_insert", bst_insert, k, 4, "4(3(2(1,.),.),.)");
}

static void ins_extremes(void)
{
    static const int k[] = { 0, INT_MIN, INT_MAX };
    tl_on_ub("Ta comparaison fait-elle un calcul qui peut dépasser les limites d'un int ?");
    check_inserts("bst_insert", bst_insert, k, 3, "0(-2147483648,2147483647)");
}

static void check_random_inserts(const char *func, insert_fn insert)
{
    enum { N = 1000 };
    int *keys = tl_ints_new(N);
    tl_rand_seed(1010);
    tl_fill_random(keys, N, -100, 100);
    struct bst_node *root = NULL;
    for (size_t i = 0; i < N; i++) {
        if (insert(&root, keys[i]) != 0) {
            keep(root);
            tl_line("1000 insertions de valeurs entre -100 et 100");
            tl_symptom("%s renvoie une erreur à l'insertion n° %zu.", func, i + 1);
            tl_question("Que doit renvoyer ta fonction quand l'allocation a réussi ?");
            tl_fail();
        }
    }
    keep(root);
    int bad = 0;
    size_t count = tl_tree_count(root, &OPS);
    if (count != N || !tl_tree_is_bst(root, &OPS, &bad)) {
        tl_line("1000 insertions de valeurs entre -100 et 100");
        if (count != N) {
            tl_symptom("l'arbre contient %zu nœuds au lieu de 1000.", count);
            tl_question("Chaque appel accroche-t-il son nouveau nœud à l'arbre ?");
        } else {
            tl_symptom("la clé %d n'est pas du bon côté d'un de ses ancêtres.", bad);
            tl_question("Quand la nouvelle clé est égale à celle du nœud, de quel côté "
                        "vas-tu ?");
        }
        tl_fail();
    }
    tl_ints_free(keys);
}

static void ins_aleatoire(void)
{
    check_random_inserts("bst_insert", bst_insert);
}

/* ---------------- Étape 2 · bst_to_array ---------------- */

/* Arbre « 5(3(1,4),8(7,9)) », construit sans malloc. */
static struct bst_node n1 = { 1, NULL, NULL };
static struct bst_node n4 = { 4, NULL, NULL };
static struct bst_node n7 = { 7, NULL, NULL };
static struct bst_node n9 = { 9, NULL, NULL };
static struct bst_node n3 = { 3, &n1, &n4 };
static struct bst_node n8 = { 8, &n7, &n9 };
static struct bst_node n5 = { 5, &n3, &n8 };

static void check_to_array(const struct bst_node *root, const char *shape, const int *expected,
                           size_t count, const int *preorder, const int *postorder)
{
    int *out = tl_ints_new(count > 0 ? count : 1);
    if (count == 0) {
        out[0] = 12345;
    }
    tl_on_crash("Ta fonction teste-t-elle si le nœud vaut NULL avant de lire ses champs ?");
    size_t got = bst_to_array(root, out);
    tl_check_guards(out, count > 0 ? count : 1, "bst_to_array");
    if (count == 0 && out[0] != 12345) {
        tl_line("arbre vide");
        tl_symptom("une case de out a été écrite alors que l'arbre est vide.");
        tl_question("Que doit faire ta fonction quand root vaut NULL ?");
        tl_fail();
    }
    if (got == count && tl_ints_equal(out, expected, count)) {
        tl_ints_free(out);
        return;
    }
    tl_line("arbre   : %s", shape);
    char note_exp[48];
    char note_got[48];
    snprintf(note_exp, sizeof note_exp, "(renvoie %zu)", count);
    snprintf(note_got, sizeof note_got, "(renvoie %zu)", got);
    tl_report_ints(NULL, expected, out, count, note_exp, note_got);
    if (preorder != NULL && got == count && tl_ints_equal(out, preorder, count)) {
        tl_symptom("c'est l'ordre préfixe : chaque nœud est écrit avant ses sous-arbres.");
        tl_question("Dans un parcours infixe, à quel moment écris-tu la clé du nœud ?");
    } else if (postorder != NULL && got == count && tl_ints_equal(out, postorder, count)) {
        tl_symptom("c'est l'ordre postfixe : chaque nœud est écrit après ses sous-arbres.");
        tl_question("Dans un parcours infixe, à quel moment écris-tu la clé du nœud ?");
    } else if (got != count) {
        tl_symptom("la fonction renvoie %zu au lieu de %zu.", got, count);
        tl_question("Si tu n'as pas commencé l'étape 2, c'est normal. Sinon : le nombre "
                    "renvoyé compte-t-il aussi les clés écrites par les appels récursifs ?");
    } else {
        tl_symptom("le bon nombre de clés, mais pas aux bonnes places.");
        tl_question("Après l'appel sur le sous-arbre gauche, à quelle position la clé du "
                    "nœud doit-elle être écrite ?");
    }
    tl_fail();
}

static void arr_vide(void)
{
    check_to_array(NULL, "(arbre vide)", NULL, 0, NULL, NULL);
}

static void arr_un(void)
{
    static const struct bst_node leaf = { 7, NULL, NULL };
    static const int expected[] = { 7 };
    check_to_array(&leaf, "7", expected, 1, NULL, NULL);
}

static void arr_exemple(void)
{
    static const int expected[] = { 1, 3, 4, 5, 7, 8, 9 };
    static const int pre[] = { 5, 3, 1, 4, 8, 7, 9 };
    static const int post[] = { 1, 4, 3, 7, 9, 8, 5 };
    check_to_array(&n5, "5(3(1,4),8(7,9))", expected, 7, pre, post);
}

static void arr_peigne_gauche(void)
{
    static const struct bst_node c1 = { 1, NULL, NULL };
    static const struct bst_node c2 = { 2, (struct bst_node *)&c1, NULL };
    static const struct bst_node c3 = { 3, (struct bst_node *)&c2, NULL };
    static const int expected[] = { 1, 2, 3 };
    check_to_array(&c3, "3(2(1,.),.)", expected, 3, NULL, NULL);
}

static void arr_peigne_droit(void)
{
    static const struct bst_node c3 = { 3, NULL, NULL };
    static const struct bst_node c2 = { 2, NULL, (struct bst_node *)&c3 };
    static const struct bst_node c1 = { 1, NULL, (struct bst_node *)&c2 };
    static const int expected[] = { 1, 2, 3 };
    check_to_array(&c1, "1(.,2(.,3))", expected, 3, NULL, NULL);
}

static void arr_doublons(void)
{
    static const struct bst_node d5b = { 5, NULL, NULL };
    static const struct bst_node d5a = { 5, NULL, (struct bst_node *)&d5b };
    static const struct bst_node d3 = { 3, NULL, NULL };
    static const struct bst_node d5 = { 5, (struct bst_node *)&d3, (struct bst_node *)&d5a };
    static const int expected[] = { 3, 5, 5, 5 };
    check_to_array(&d5, "5(3,5(.,5))", expected, 4, NULL, NULL);
}

/* ---------------- Étape 3 · bst_free ---------------- */

static struct bst_node *build(const int *keys, size_t k)
{
    struct bst_node *root = NULL;
    for (size_t i = 0; i < k; i++) {
        if (bst_insert(&root, keys[i]) != 0) {
            tl_line("construction de l'arbre avec bst_insert");
            tl_symptom("bst_insert renvoie une erreur.");
            tl_question("Tes tests de l'étape 1 passent-ils tous ?");
            tl_fail();
        }
    }
    return root;
}

static void free_vide(void)
{
    tl_on_crash("Ta fonction teste-t-elle si le nœud vaut NULL avant de lire ses champs ?");
    bst_free(NULL);
}

static void free_exemple(void)
{
    static const int k[] = { 5, 3, 8, 1, 4, 9, 7 };
    struct bst_node *root = build(k, 7);
    tl_on_crash("Lis-tu les enfants d'un nœud après l'avoir rendu avec free ?");
    bst_free(root);
}

static void free_grand(void)
{
    enum { N = 10000 };
    int *keys = tl_ints_new(N);
    tl_rand_seed(1011);
    tl_fill_random(keys, N, INT_MIN, INT_MAX);
    struct bst_node *root = build(keys, N);
    tl_on_crash("Lis-tu les enfants d'un nœud après l'avoir rendu avec free ?");
    bst_free(root);
    tl_ints_free(keys);
}

/* ---------------- Étape 4 · tree_sort ---------------- */

TL_SORT_SUITE_DEFINE(tree_sort, 100000)

/* ---------------- Bonus · bst_insert_iter ---------------- */

static void iter_exemple(void)
{
    static const int k[] = { 5, 3, 8, 1, 4, 9, 7 };
    check_inserts("bst_insert_iter", bst_insert_iter, k, 7, "5(3(1,4),8(7,9))");
}

static void iter_vide(void)
{
    static const int k[] = { 7 };
    check_inserts("bst_insert_iter", bst_insert_iter, k, 1, "7");
}

static void iter_egaux(void)
{
    static const int k[] = { 5, 5, 3, 5 };
    check_inserts("bst_insert_iter", bst_insert_iter, k, 4, "5(3,5(.,5))");
}

static void iter_croissant(void)
{
    static const int k[] = { 1, 2, 3, 4 };
    check_inserts("bst_insert_iter", bst_insert_iter, k, 4, "1(.,2(.,3(.,4)))");
}

static void iter_aleatoire(void)
{
    check_random_inserts("bst_insert_iter", bst_insert_iter);
}

/* ---------------- Tables ---------------- */

static const tl_step STEPS[] = {
    { 1, "bst_insert", false },
    { 2, "bst_to_array", false },
    { 3, "bst_free", false },
    { 4, "tree_sort", false },
    { 5, "bst_insert_iter", true },
};

static const tl_test TESTS[] = {
    TL_TEST(1, ins_vide, "insérer 7 dans un arbre vide"),
    TL_TEST(1, ins_trois, "insérer 5, 3, 8"),
    TL_TEST(1, ins_exemple, "insérer 5, 3, 8, 1, 4, 9, 7"),
    TL_TEST(1, ins_egaux, "insérer 5, 5, 3, 5"),
    TL_TEST(1, ins_croissant, "insérer 1, 2, 3, 4"),
    TL_TEST(1, ins_decroissant, "insérer 4, 3, 2, 1"),
    TL_TEST(1, ins_extremes, "insérer 0, INT_MIN, INT_MAX"),
    TL_TEST(1, ins_aleatoire, "1000 insertions"),

    TL_TEST(2, arr_vide, "arbre vide"),
    TL_TEST(2, arr_un, "un seul nœud"),
    TL_TEST(2, arr_exemple, "arbre 5(3(1,4),8(7,9))"),
    TL_TEST(2, arr_peigne_gauche, "que des fils gauches"),
    TL_TEST(2, arr_peigne_droit, "que des fils droits"),
    TL_TEST(2, arr_doublons, "clés égales"),

    TL_TEST(3, free_vide, "arbre vide"),
    TL_TEST(3, free_exemple, "7 nœuds (fuites vues par make check)"),
    TL_TEST(3, free_grand, "10 000 nœuds (fuites vues par make check)"),

    TL_SORT_SUITE_ENTRIES(4, tree_sort),

    TL_TEST(5, iter_vide, "insérer 7 dans un arbre vide"),
    TL_TEST(5, iter_exemple, "insérer 5, 3, 8, 1, 4, 9, 7"),
    TL_TEST(5, iter_egaux, "insérer 5, 5, 3, 5"),
    TL_TEST(5, iter_croissant, "insérer 1, 2, 3, 4"),
    TL_TEST(5, iter_aleatoire, "1000 insertions"),
};

TL_MAIN("10", "Tri par arbre binaire de recherche", STEPS, TESTS)

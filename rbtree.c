#include "rbtree.h"
#include <stdlib.h>
#include <string.h>

static Node *create_node(const char *key, double value) {
    Node *node = (Node *)malloc(sizeof(Node));
    strcpy(node->key, key);
    node->value = value;
    node->left = node->right = node->parent = NULL;
    node->color = RED;
    return node;
}

static void left_rotate(RBTree *tree, Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

static void right_rotate(RBTree *tree, Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right) y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent)
        tree->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
}

static void fix_insert(RBTree *tree, Node *z) {
    while (z->parent && z->parent->color == RED) {
        Node *grandparent = z->parent->parent;
        if (z->parent == grandparent->left) {
            Node *uncle = grandparent->right;
            if (uncle && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(tree, z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                right_rotate(tree, grandparent);
            }
        } else {
            Node *uncle = grandparent->left;
            if (uncle && uncle->color == RED) {
                z->parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                z = grandparent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(tree, z);
                }
                z->parent->color = BLACK;
                grandparent->color = RED;
                left_rotate(tree, grandparent);
            }
        }
    }
    tree->root->color = BLACK;
}

void rbtree_insert(RBTree *tree, const char *key, double value) {
    if (!tree) return;
    
    Node *z = create_node(key, value);
    Node *y = NULL;
    Node *x = tree->root;

    while (x) {
        y = x;
        int cmp = strcmp(key, x->key);
        if (cmp < 0)
            x = x->left;
        else if (cmp > 0)
            x = x->right;
        else {
            x->value = value;
            free(z);
            return;
        }
    }

    z->parent = y;
    if (!y)
        tree->root = z;
    else if (strcmp(key, y->key) < 0)
        y->left = z;
    else
        y->right = z;

    fix_insert(tree, z);
}

static Node *search_node(Node *root, const char *key) {
    if (!root) return NULL;
    int cmp = strcmp(key, root->key);
    if (cmp < 0) return search_node(root->left, key);
    if (cmp > 0) return search_node(root->right, key);
    return root;
}

int rbtree_search(RBTree *tree, const char *key, double *value) {
    Node *node = search_node(tree->root, key);
    if (node) {
        if (value) *value = node->value;
        return 1;
    }
    return 0;
}

static Node *minimum(Node *node) {
    while (node->left) node = node->left;
    return node;
}

static void transplant(RBTree *tree, Node *u, Node *v) {
    if (!u->parent)
        tree->root = v;
    else if (u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;
    if (v) v->parent = u->parent;
}

static void fix_delete(RBTree *tree, Node *x) {
    while (x != tree->root && (!x || x->color == BLACK)) {
        if (x == x->parent->left) {
            Node *w = x->parent->right;
            if (w && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(tree, x->parent);
                w = x->parent->right;
            }
            if (w && (!w->left || w->left->color == BLACK) && 
                (!w->right || w->right->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w && (!w->right || w->right->color == BLACK)) {
                    if (w->left) w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x->parent->right;
                }
                if (w) {
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->right) w->right->color = BLACK;
                    left_rotate(tree, x->parent);
                }
                x = tree->root;
            }
        } else {
            Node *w = x->parent->left;
            if (w && w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(tree, x->parent);
                w = x->parent->left;
            }
            if (w && (!w->right || w->right->color == BLACK) && 
                (!w->left || w->left->color == BLACK)) {
                w->color = RED;
                x = x->parent;
            } else {
                if (w && (!w->left || w->left->color == BLACK)) {
                    if (w->right) w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(tree, w);
                    w = x->parent->left;
                }
                if (w) {
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->left) w->left->color = BLACK;
                    right_rotate(tree, x->parent);
                }
                x = tree->root;
            }
        }
    }
    if (x) x->color = BLACK;
}

void rbtree_delete(RBTree *tree, const char *key) {
    if (!tree || !tree->root) return;
    
    Node *z = search_node(tree->root, key);
    if (!z) return;

    Node *y = z;
    Node *x;
    Color y_original_color = y->color;

    if (!z->left) {
        x = z->right;
        transplant(tree, z, z->right);
    } else if (!z->right) {
        x = z->left;
        transplant(tree, z, z->left);
    } else {
        y = minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            if (x) x->parent = y;
        } else {
            transplant(tree, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        transplant(tree, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    free(z);
    
    if (y_original_color == BLACK && x)
        fix_delete(tree, x);
}

static void print_tree_rec(Node *root, int level, FILE *out) {
    if (!root) return;
    print_tree_rec(root->right, level + 1, out);
    for (int i = 0; i < level; i++) fprintf(out, "    ");
    char color_char = (root->color == RED) ? 'R' : 'B';
    fprintf(out, "%s[%c] (%.2f)\n", root->key, color_char, root->value);
    print_tree_rec(root->left, level + 1, out);
}

void rbtree_print(RBTree *tree, FILE *out) {
    if (!tree) return;
    fprintf(out, "Tree (R=red, B=black):\n");
    print_tree_rec(tree->root, 0, out);
    fprintf(out, "\n");
}

static void free_node(Node *node) {
    if (!node) return;
    free_node(node->left);
    free_node(node->right);
    free(node);
}

void rbtree_free(RBTree *tree) {
    if (!tree) return;
    free_node(tree->root);
    tree->root = NULL;
}

void rbtree_init(RBTree *tree) {
    if (tree) tree->root = NULL;
}

int valid_key(const char *key) {
    int len = strlen(key);
    if (len == 0 || len > MAX_KEY) return 0;
    for (int i = 0; i < len; i++)
        if (!((key[i] >= 'a' && key[i] <= 'z') || 
              (key[i] >= 'A' && key[i] <= 'Z')))
            return 0;
    return 1;
}
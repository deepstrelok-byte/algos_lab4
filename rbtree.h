#ifndef RBTREE_H
#define RBTREE_H

#include <stdio.h>

#define MAX_KEY 6

typedef enum { RED, BLACK } Color;

typedef struct Node {
    char key[MAX_KEY + 1];
    double value;
    struct Node *left;
    struct Node *right;
    struct Node *parent;
    Color color;
} Node;

typedef struct {
    Node *root;
} RBTree;

// Инициализация дерева
void rbtree_init(RBTree *tree);

// Вставка узла
void rbtree_insert(RBTree *tree, const char *key, double value);

// Удаление узла
void rbtree_delete(RBTree *tree, const char *key);

// Поиск узла (возвращает значение через указатель, 1 если найден)
int rbtree_search(RBTree *tree, const char *key, double *value);

// Печать дерева
void rbtree_print(RBTree *tree, FILE *out);

// Освобождение памяти
void rbtree_free(RBTree *tree);

// Проверка ключа
int valid_key(const char *key);

#endif
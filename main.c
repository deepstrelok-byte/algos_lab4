#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256

static void trim(char *str) {
    char *p = str;
    while (*p == ' ' || *p == '\t') p++;
    if (p != str) memmove(str, p, strlen(p) + 1);
    
    char *end = str + strlen(str) - 1;
    while (end > str && (*end == ' ' || *end == '\t' || *end == '\n')) end--;
    *(end + 1) = '\0';
}

static int parse_command(char *line, int *cmd, char *key, double *val) {
    char temp_key[MAX_KEY + 1];
    double temp_val;
    int temp_cmd;
    
    // Проверка на команду выхода
    if (strcmp(line, "exit") == 0 || strcmp(line, "quit") == 0) {
        *cmd = 5;  // Специальный код для выхода
        return 1;
    }
    
    if (sscanf(line, "%d", &temp_cmd) != 1) return 0;
    
    switch (temp_cmd) {
        case 1:
            if (sscanf(line, "%d %6s %lf", &temp_cmd, temp_key, &temp_val) == 3) {
                *cmd = temp_cmd;
                strcpy(key, temp_key);
                *val = temp_val;
                return 1;
            }
            break;
        case 2:
        case 4:
            if (sscanf(line, "%d %6s", &temp_cmd, temp_key) == 2) {
                *cmd = temp_cmd;
                strcpy(key, temp_key);
                return 1;
            }
            break;
        case 3:
            *cmd = 3;
            return 1;
        default:
            return 0;
    }
    return 0;
}

static void print_help(FILE *out) {
    fprintf(out, "\n=== RB-Tree Commands ===\n");
    fprintf(out, "1 <key> <value>  - Insert node\n");
    fprintf(out, "2 <key>          - Delete node\n");
    fprintf(out, "3                - Print tree\n");
    fprintf(out, "4 <key>          - Search node\n");
    fprintf(out, "exit or quit     - Exit program\n");
    fprintf(out, "help             - Show this help\n");
    fprintf(out, "========================\n\n");
}

int main(int argc, char *argv[]) {
    RBTree tree;
    rbtree_init(&tree);
    
    FILE *in = stdin;
    FILE *out = stdout;
    int interactive = 1;

    if (argc >= 2) {
        in = fopen(argv[1], "r");
        if (!in) {
            fprintf(stderr, "Cannot open input file: %s\n", argv[1]);
            return 1;
        }
        interactive = 0;
    }
    if (argc >= 3) {
        out = fopen(argv[2], "w");
        if (!out) {
            fprintf(stderr, "Cannot open output file: %s\n", argv[2]);
            return 1;
        }
    }

    if (interactive) {
        printf("RB-Tree Interactive Mode\n");
        print_help(stdout);
    }

    char line[MAX_LINE];
    int cmd;
    char key[MAX_KEY + 1];
    double val;
    int line_num = 0;

    while (1) {
        if (interactive) {
            printf("> ");
            fflush(stdout);
        }
        
        if (!fgets(line, sizeof(line), in)) break;
        
        line_num++;
        trim(line);
        
        // Пропускаем пустые строки и комментарии в файловом режиме
        if (line[0] == '\0' || line[0] == '#') {
            if (!interactive && line[0] != '\0') {
                fprintf(out, "%s\n", line);
            }
            continue;
        }
        
        // Обработка команды help
        if (interactive && strcmp(line, "help") == 0) {
            print_help(out);
            continue;
        }
        
        if (!parse_command(line, &cmd, key, &val)) {
            if (interactive) {
                printf("Invalid command. Type 'help' for available commands.\n");
            } else {
                fprintf(out, "%s\n", line);
                fprintf(out, "Invalid command at line %d\n\n", line_num);
            }
            continue;
        }
        
        // Выход из программы
        if (cmd == 5) {
            if (interactive) {
                printf("Goodbye!\n");
            }
            break;
        }
        
        // В файловом режиме записываем команду
        if (!interactive) {
            fprintf(out, "%s\n", line);
        }
        
        switch (cmd) {
            case 1:
                if (valid_key(key)) {
                    rbtree_insert(&tree, key, val);
                    if (interactive) {
                        printf("Inserted: %s = %.2f\n", key, val);
                    } else {
                        fprintf(out, "Inserted: %s = %.2f\n\n", key, val);
                    }
                } else {
                    char msg[256];
                    sprintf(msg, "Invalid key (must be 1-6 letters): %s", key);
                    if (interactive) {
                        printf("%s\n", msg);
                    } else {
                        fprintf(out, "%s\n\n", msg);
                    }
                }
                break;
            case 2:
                if (valid_key(key)) {
                    rbtree_delete(&tree, key);
                    if (interactive) {
                        printf("Deleted: %s\n", key);
                    } else {
                        fprintf(out, "Deleted: %s\n\n", key);
                    }
                } else {
                    char msg[256];
                    sprintf(msg, "Invalid key: %s", key);
                    if (interactive) {
                        printf("%s\n", msg);
                    } else {
                        fprintf(out, "%s\n\n", msg);
                    }
                }
                break;
            case 3:
                rbtree_print(&tree, out);
                if (interactive && out == stdout) {
                    // Уже вывели
                }
                break;
            case 4:
                if (valid_key(key)) {
                    double found_val;
                    if (rbtree_search(&tree, key, &found_val)) {
                        if (interactive) {
                            printf("Found: %s = %.2f\n", key, found_val);
                        } else {
                            fprintf(out, "Found: %s = %.2f\n\n", key, found_val);
                        }
                    } else {
                        if (interactive) {
                            printf("Not found: %s\n", key);
                        } else {
                            fprintf(out, "Not found: %s\n\n", key);
                        }
                    }
                } else {
                    char msg[256];
                    sprintf(msg, "Invalid key: %s", key);
                    if (interactive) {
                        printf("%s\n", msg);
                    } else {
                        fprintf(out, "%s\n\n", msg);
                    }
                }
                break;
            default:
                if (interactive) {
                    printf("Unknown command\n");
                } else {
                    fprintf(out, "Unknown command\n\n");
                }
        }
    }

    if (!interactive && in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    rbtree_free(&tree);
    
    return 0;
}
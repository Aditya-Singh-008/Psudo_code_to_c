#include "symtab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOLS 100

struct Symbol {
    char *name;
    char *type;
};

static struct Symbol symbol_table[MAX_SYMBOLS];
static int symbol_count = 0;

int add_symbol(char *name, char *type) {
    /* Reject duplicates */
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            fprintf(stderr, "Error: Variable '%s' already declared.\n", name);
            return -1;
        }
    }
    /* Reject overflow */
    if (symbol_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: Symbol table full. Cannot declare '%s'.\n", name);
        return -2;
    }
    symbol_table[symbol_count].name = strdup(name);
    symbol_table[symbol_count].type = strdup(type);
    symbol_count++;
    return 0;
}

char* get_type_sym(char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0)
            return symbol_table[i].type;
    }
    return NULL;
}

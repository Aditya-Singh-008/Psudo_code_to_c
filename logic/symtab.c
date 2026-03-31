#include "symtab.h"
#include <string.h>
#include <stdlib.h>

struct Symbol {
    char *name;
    char *type;
};

struct Symbol symbol_table[100];
int symbol_count = 0;

void add_symbol(char *name, char *type) {
    if (symbol_count < 100) {
        symbol_table[symbol_count].name = strdup(name);
        symbol_table[symbol_count].type = strdup(type);
        symbol_count++;
    }
}

char* get_type_sym(char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) return symbol_table[i].type;
    }
    return NULL;
}

#ifndef SYMTAB_H
#define SYMTAB_H

/* Returns  0 on success.
 * Returns -1 if name is already declared (duplicate).
 * Returns -2 if the symbol table is full.            */
int  add_symbol(char *name, char *type);
char* get_type_sym(char *name);

#endif

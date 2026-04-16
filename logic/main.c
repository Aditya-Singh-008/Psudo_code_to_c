#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "codegen.h"

extern int yyparse();
ASTNode* ast_root = NULL;

int main(int argc, char** argv) {
    if (yyparse() == 0) {
        if (ast_root != NULL) {
            if (argc > 1 && strcmp(argv[1], "python") == 0) {
                generate_code_python(ast_root, 0);
            } else if (argc > 1 && strcmp(argv[1], "java") == 0) {
                generate_code_java(ast_root, 0);
            } else {
                generate_code_c(ast_root, 0);
            }
            free_ast(ast_root);   /* Release all AST heap memory */
            ast_root = NULL;
        }
    } else {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }
    return 0;
}

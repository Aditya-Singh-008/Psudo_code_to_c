#include <stdio.h>
#include "ast.h"
#include "codegen.h"

extern int yyparse();
ASTNode* ast_root = NULL;

int main() {
    if (yyparse() == 0) {
        if (ast_root != NULL) {
            /* Pass indent_level=0 so top-level statements get one level (1) of indentation,
               not two. Previously called with 1 which caused double-indentation. */
            generate_code(ast_root, 0);
            free_ast(ast_root);   /* Release all AST heap memory */
            ast_root = NULL;
        }
    } else {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }
    return 0;
}

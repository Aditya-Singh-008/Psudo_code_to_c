#include <stdio.h>
#include "ast.h"
#include "codegen.h"

extern int yyparse();
ASTNode* ast_root = NULL;

int main() {
    if (yyparse() == 0) {
        if (ast_root != NULL) {
            generate_code(ast_root, 1);
        }
    } else {
        fprintf(stderr, "Parsing failed.\n");
        return 1;
    }
    return 0;
}

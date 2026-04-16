#include "codegen.h"
#include "symtab.h"
#include <stdio.h>

void print_indent(int level) {
    for(int i = 0; i < level; i++) {
        printf("    ");
    }
}

void generate_expr(ASTNode* expr) {
    if (!expr) return;
    if (expr->type == NODE_NUM) {
        printf("%d", expr->int_val);
    } else if (expr->type == NODE_STR) {
        printf("%s", expr->str_val);
    } else if (expr->type == NODE_ID) {
        /* Warn if variable was never declared */
        if (get_type_sym(expr->str_val) == NULL) {
            fprintf(stderr, "Warning: Undeclared variable '%s' used in expression.\n", expr->str_val);
        }
        printf("%s", expr->str_val);
    } else if (expr->type == NODE_ARRAY_ACCESS) {
        /* arr[index] — emit as-is; symtab check for the array name */
        if (get_type_sym(expr->str_val) == NULL) {
            fprintf(stderr, "Warning: Undeclared array '%s' used in expression.\n", expr->str_val);
        }
        printf("%s[", expr->str_val);
        generate_expr(expr->left);   /* index expression */
        printf("]");
    } else if (expr->type == NODE_BINOP) {
        printf("(");
        generate_expr(expr->left);
        printf(" %s ", expr->str_val);
        generate_expr(expr->right);
        printf(")");
    }
}

void generate_code_c(ASTNode* node, int indent_level) {
    if (node == NULL) return;

    if (node->type == NODE_PROGRAM) {
        printf("#include <stdio.h>\n\nint main() {\n");
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code_c(stmt, indent_level + 1);
            stmt = stmt->next;
        }
        printf("    return 0;\n}\n");
    }
    else if (node->type == NODE_BLOCK) {
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code_c(stmt, indent_level);
            stmt = stmt->next;
        }
    }
    else if (node->type == NODE_VAR_DECL) {
        print_indent(indent_level);
        printf("int %s;\n", node->str_val);
    }
    else if (node->type == NODE_ARRAY_DECL) {
        /* VAR arr AS INT ARRAY SIZE n  →  int arr[n] = {0}; */
        print_indent(indent_level);
        printf("int %s[%d] = {0};\n", node->str_val, node->int_val);
    }
    else if (node->type == NODE_ASSIGN) {
        /* Warn if assigning to a variable that was never declared */
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: Assignment to undeclared variable '%s'.\n", node->str_val);
        }
        print_indent(indent_level);
        printf("%s = ", node->str_val);
        generate_expr(node->left);
        printf(";\n");
    }
    else if (node->type == NODE_ARRAY_ASSIGN) {
        /* SET arr[index] TO expr  →  arr[index] = expr; */
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: Assignment to undeclared array '%s'.\n", node->str_val);
        }
        print_indent(indent_level);
        printf("%s[", node->str_val);
        generate_expr(node->left);    /* index */
        printf("] = ");
        generate_expr(node->right);   /* value */
        printf(";\n");
    }
    else if (node->type == NODE_PRINT) {
        print_indent(indent_level);
        if (node->left->type == NODE_STR) {
            printf("printf(\"%%s\\n\", %s);\n", node->left->str_val);
        } else if (node->left->type == NODE_ID) {
            if (get_type_sym(node->left->str_val) == NULL) {
                fprintf(stderr, "Warning: SHOW used on undeclared variable '%s'.\n", node->left->str_val);
            }
            printf("printf(\"%%d\\n\", %s);\n", node->left->str_val);
        } else {
            /* Handles NODE_ARRAY_ACCESS and any other numeric expression */
            printf("printf(\"%%d\\n\", ");
            generate_expr(node->left);
            printf(");\n");
        }
    }
    else if (node->type == NODE_INPUT) {
        /* Warn if reading into a variable that was never declared */
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: GET used on undeclared variable '%s'.\n", node->str_val);
        }
        print_indent(indent_level);
        printf("scanf(\"%%d\", &%s);\n", node->str_val);
    }
    else if (node->type == NODE_ARRAY_INPUT) {
        /* GET arr[index]  →  scanf("%d", &arr[index]); */
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: GET used on undeclared array '%s'.\n", node->str_val);
        }
        print_indent(indent_level);
        printf("scanf(\"%%d\", &%s[", node->str_val);
        generate_expr(node->left);   /* index */
        printf("]);\n");
    }
    else if (node->type == NODE_IF) {
        print_indent(indent_level);
        printf("if (");
        generate_expr(node->cond);
        printf(") {\n");

        ASTNode* stmt = node->then_branch;
        while (stmt) {
            generate_code_c(stmt, indent_level + 1);
            stmt = stmt->next;
        }

        print_indent(indent_level);
        printf("}\n");
        if (node->else_branch) {
            print_indent(indent_level);
            printf("else {\n");

            stmt = node->else_branch;
            while (stmt) {
                generate_code_c(stmt, indent_level + 1);
                stmt = stmt->next;
            }

            print_indent(indent_level);
            printf("}\n");
        }
    }
    else if (node->type == NODE_WHILE) {
        print_indent(indent_level);
        printf("while (");
        generate_expr(node->cond);
        printf(") {\n");

        ASTNode* stmt = node->then_branch;
        while (stmt) {
            generate_code_c(stmt, indent_level + 1);
            stmt = stmt->next;
        }

        print_indent(indent_level);
        printf("}\n");
    }
}

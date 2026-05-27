#include "codegen.h"
#include "symtab.h"
#include <stdio.h>

static void print_indent_py(int level) {
    for(int i = 0; i < level; i++) {
        printf("    ");
    }
}

static void generate_expr_py(ASTNode* expr) {
    if (!expr) return;
    if (expr->type == NODE_NUM) {
        printf("%d", expr->int_val);
    } else if (expr->type == NODE_STR) {
        printf("%s", expr->str_val);
    } else if (expr->type == NODE_ID) {
        if (get_type_sym(expr->str_val) == NULL) {
            fprintf(stderr, "Warning: Undeclared variable '%s' used in expression.\n", expr->str_val);
        }
        printf("%s", expr->str_val);
    } else if (expr->type == NODE_ARRAY_ACCESS) {
        if (get_type_sym(expr->str_val) == NULL) {
            fprintf(stderr, "Warning: Undeclared array '%s' used in expression.\n", expr->str_val);
        }
        printf("%s[", expr->str_val);
        generate_expr_py(expr->left);
        printf("]");
    } else if (expr->type == NODE_BINOP) {
        printf("(");
        generate_expr_py(expr->left);
        printf(" %s ", expr->str_val);
        generate_expr_py(expr->right);
        printf(")");
    }
}

void generate_code_python(ASTNode* node, int indent_level) {
    if (node == NULL) return;

    if (node->type == NODE_PROGRAM) {
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code_python(stmt, indent_level);
            stmt = stmt->next;
        }
    }
    else if (node->type == NODE_BLOCK) {
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code_python(stmt, indent_level);
            stmt = stmt->next;
        }
    }
    else if (node->type == NODE_VAR_DECL) {
        /* In Python there are no type declarations.
           We emit `name = 0` as a stub ONLY if the very next statement
           is NOT already a SET (NODE_ASSIGN) to the same name — otherwise
           we get the redundant double-assignment. */
        int skip = (node->next != NULL &&
                    node->next->type == NODE_ASSIGN &&
                    node->next->str_val != NULL &&
                    strcmp(node->next->str_val, node->str_val) == 0);
        if (!skip) {
            print_indent_py(indent_level);
            printf("%s = 0\n", node->str_val);
        }
    }

    else if (node->type == NODE_ARRAY_DECL) {
        print_indent_py(indent_level);
        printf("%s = [0] * %d\n", node->str_val, node->int_val);
    }
    else if (node->type == NODE_ARRAY_DECL_INIT) {
        /* arr = [10, 20, 30] */
        print_indent_py(indent_level);
        printf("%s = [", node->str_val);
        for (int i = 0; i < node->values_len; i++) {
            if (i > 0) printf(", ");
            printf("%d", node->values[i]);
        }
        printf("]\n");
    }
    else if (node->type == NODE_ARRAY_DECL_DYNAMIC) {
        /* Python lists are already dynamic — start empty */
        print_indent_py(indent_level);
        printf("%s = []  # dynamic list\n", node->str_val);
    }

    else if (node->type == NODE_ASSIGN) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: Assignment to undeclared variable '%s'.\n", node->str_val);
        }
        print_indent_py(indent_level);
        printf("%s = ", node->str_val);
        generate_expr_py(node->left);
        printf("\n");
    }
    else if (node->type == NODE_ARRAY_ASSIGN) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: Assignment to undeclared array '%s'.\n", node->str_val);
        }
        print_indent_py(indent_level);
        printf("%s[", node->str_val);
        generate_expr_py(node->left);
        printf("] = ");
        generate_expr_py(node->right);
        printf("\n");
    }
    else if (node->type == NODE_PRINT) {
        print_indent_py(indent_level);
        if (node->left->type == NODE_ID && get_type_sym(node->left->str_val) == NULL) {
            fprintf(stderr, "Warning: SHOW used on undeclared variable '%s'.\n", node->left->str_val);
        }
        printf("print(");
        generate_expr_py(node->left);
        printf(")\n");
    }
    else if (node->type == NODE_INPUT) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: GET used on undeclared variable '%s'.\n", node->str_val);
        }
        print_indent_py(indent_level);
        printf("%s = int(input())\n", node->str_val);
    }
    else if (node->type == NODE_ARRAY_INPUT) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: GET used on undeclared array '%s'.\n", node->str_val);
        }
        print_indent_py(indent_level);
        printf("%s[", node->str_val);
        generate_expr_py(node->left);
        printf("] = int(input())\n");
    }
    else if (node->type == NODE_IF) {
        print_indent_py(indent_level);
        printf("if ");
        generate_expr_py(node->cond);
        printf(":\n");

        ASTNode* stmt = node->then_branch;
        if (!stmt) {
            print_indent_py(indent_level + 1);
            printf("pass\n");
        }
        while (stmt) {
            generate_code_python(stmt, indent_level + 1);
            stmt = stmt->next;
        }

        if (node->else_branch) {
            print_indent_py(indent_level);
            printf("else:\n");
            stmt = node->else_branch;
            if (!stmt) {
                print_indent_py(indent_level + 1);
                printf("pass\n");
            }
            while (stmt) {
                generate_code_python(stmt, indent_level + 1);
                stmt = stmt->next;
            }
        }
    }
    else if (node->type == NODE_WHILE) {
        print_indent_py(indent_level);
        printf("while ");
        generate_expr_py(node->cond);
        printf(":\n");

        ASTNode* stmt = node->then_branch;
        if (!stmt) {
            print_indent_py(indent_level + 1);
            printf("pass\n");
        }
        while (stmt) {
            generate_code_python(stmt, indent_level + 1);
            stmt = stmt->next;
        }
    }
}

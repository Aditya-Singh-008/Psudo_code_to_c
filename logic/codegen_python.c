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
        printf("%s", expr->str_val);
    } else if (expr->type == NODE_ARRAY_ACCESS) {
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
        print_indent_py(indent_level);
        printf("%s = 0\n", node->str_val);
    }
    else if (node->type == NODE_ARRAY_DECL) {
        print_indent_py(indent_level);
        printf("%s = [0] * %d\n", node->str_val, node->int_val);
    }
    else if (node->type == NODE_ASSIGN) {
        print_indent_py(indent_level);
        printf("%s = ", node->str_val);
        generate_expr_py(node->left);
        printf("\n");
    }
    else if (node->type == NODE_ARRAY_ASSIGN) {
        print_indent_py(indent_level);
        printf("%s[", node->str_val);
        generate_expr_py(node->left);
        printf("] = ");
        generate_expr_py(node->right);
        printf("\n");
    }
    else if (node->type == NODE_PRINT) {
        print_indent_py(indent_level);
        printf("print(");
        generate_expr_py(node->left);
        printf(")\n");
    }
    else if (node->type == NODE_INPUT) {
        print_indent_py(indent_level);
        printf("%s = int(input())\n", node->str_val);
    }
    else if (node->type == NODE_ARRAY_INPUT) {
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

#include "codegen.h"
#include "symtab.h"
#include <stdio.h>

static void print_indent_java(int level) {
    for(int i = 0; i < level; i++) {
        printf("    ");
    }
}

static void generate_expr_java(ASTNode* expr) {
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
        generate_expr_java(expr->left);
        printf("]");
    } else if (expr->type == NODE_BINOP) {
        printf("(");
        generate_expr_java(expr->left);
        printf(" %s ", expr->str_val);
        generate_expr_java(expr->right);
        printf(")");
    }
}

void generate_code_java(ASTNode* node, int indent_level) {
    if (node == NULL) return;

    if (node->type == NODE_PROGRAM) {
        printf("import java.util.Scanner;\nimport java.util.ArrayList;\n\n");
        printf("public class Main {\n");
        printf("    public static void main(String[] args) {\n");
        printf("        Scanner scanner = new Scanner(System.in);\n");
        
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code_java(stmt, 2); /* Main block is at indent level 2 */
            stmt = stmt->next;
        }
        
        printf("        scanner.close();\n");
        printf("    }\n");
        printf("}\n");
    }
    else if (node->type == NODE_BLOCK) {
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code_java(stmt, indent_level);
            stmt = stmt->next;
        }
    }
    else if (node->type == NODE_VAR_DECL) {
        print_indent_java(indent_level);
        printf("int %s;\n", node->str_val);
    }

    else if (node->type == NODE_ARRAY_DECL) {
        print_indent_java(indent_level);
        printf("int[] %s = new int[%d];\n", node->str_val, node->int_val);
    }
    else if (node->type == NODE_ARRAY_DECL_INIT) {
        /* int[] arr = {10, 20, 30}; */
        print_indent_java(indent_level);
        printf("int[] %s = {", node->str_val);
        for (int i = 0; i < node->values_len; i++) {
            if (i > 0) printf(", ");
            printf("%d", node->values[i]);
        }
        printf("};\n");
    }
    else if (node->type == NODE_ARRAY_DECL_DYNAMIC) {
        /* ArrayList<Integer> arr = new ArrayList<>(); */
        print_indent_java(indent_level);
        if (node->int_val > 0)
            printf("ArrayList<Integer> %s = new ArrayList<>(%d);\n", node->str_val, node->int_val);
        else
            printf("ArrayList<Integer> %s = new ArrayList<>();\n", node->str_val);
    }

    else if (node->type == NODE_ASSIGN) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: Assignment to undeclared variable '%s'.\n", node->str_val);
        }
        print_indent_java(indent_level);
        printf("%s = ", node->str_val);
        generate_expr_java(node->left);
        printf(";\n");
    }
    else if (node->type == NODE_ARRAY_ASSIGN) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: Assignment to undeclared array '%s'.\n", node->str_val);
        }
        print_indent_java(indent_level);
        printf("%s[", node->str_val);
        generate_expr_java(node->left);
        printf("] = ");
        generate_expr_java(node->right);
        printf(";\n");
    }
    else if (node->type == NODE_PRINT) {
        print_indent_java(indent_level);
        if (node->left->type == NODE_ID && get_type_sym(node->left->str_val) == NULL) {
            fprintf(stderr, "Warning: SHOW used on undeclared variable '%s'.\n", node->left->str_val);
        }
        printf("System.out.println(");
        generate_expr_java(node->left);
        printf(");\n");
    }
    else if (node->type == NODE_INPUT) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: GET used on undeclared variable '%s'.\n", node->str_val);
        }
        print_indent_java(indent_level);
        printf("%s = scanner.nextInt();\n", node->str_val);
    }
    else if (node->type == NODE_ARRAY_INPUT) {
        if (get_type_sym(node->str_val) == NULL) {
            fprintf(stderr, "Warning: GET used on undeclared array '%s'.\n", node->str_val);
        }
        print_indent_java(indent_level);
        printf("%s[", node->str_val);
        generate_expr_java(node->left);
        printf("] = scanner.nextInt();\n");
    }
    else if (node->type == NODE_IF) {
        print_indent_java(indent_level);
        printf("if (");
        generate_expr_java(node->cond);
        printf(") {\n");

        ASTNode* stmt = node->then_branch;
        while (stmt) {
            generate_code_java(stmt, indent_level + 1);
            stmt = stmt->next;
        }

        print_indent_java(indent_level);
        printf("}\n");
        if (node->else_branch) {
            print_indent_java(indent_level);
            printf("else {\n");

            stmt = node->else_branch;
            while (stmt) {
                generate_code_java(stmt, indent_level + 1);
                stmt = stmt->next;
            }

            print_indent_java(indent_level);
            printf("}\n");
        }
    }
    else if (node->type == NODE_WHILE) {
        print_indent_java(indent_level);
        printf("while (");
        generate_expr_java(node->cond);
        printf(") {\n");

        ASTNode* stmt = node->then_branch;
        while (stmt) {
            generate_code_java(stmt, indent_level + 1);
            stmt = stmt->next;
        }

        print_indent_java(indent_level);
        printf("}\n");
    }
}

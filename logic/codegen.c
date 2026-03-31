#include "codegen.h"
#include <stdio.h>

void print_indent(int level) {
    for(int i=0; i<level; i++) {
        printf("    ");
    }
}

void generate_expr(ASTNode* expr) {
    if (!expr) return;
    if (expr->type == NODE_NUM) {
        printf("%d", expr->int_val);
    } else if (expr->type == NODE_ID || expr->type == NODE_STR) {
        printf("%s", expr->str_val);
    } else if (expr->type == NODE_BINOP) {
        printf("(");
        generate_expr(expr->left);
        printf(" %s ", expr->str_val);
        generate_expr(expr->right);
        printf(")");
    }
}

void generate_code(ASTNode* node, int indent_level) {
    if (node == NULL) return;

    if (node->type == NODE_PROGRAM) {
        printf("#include <stdio.h>\n\nint main() {\n");
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code(stmt, indent_level + 1);
            stmt = stmt->next;
        }
        printf("    return 0;\n}\n");
    } 
    else if (node->type == NODE_BLOCK) {
        ASTNode* stmt = node->left;
        while (stmt != NULL) {
            generate_code(stmt, indent_level);
            stmt = stmt->next;
        }
    }
    else if (node->type == NODE_VAR_DECL) {
        print_indent(indent_level);
        printf("int %s;\n", node->str_val);
    }
    else if (node->type == NODE_ASSIGN) {
        print_indent(indent_level);
        printf("%s = ", node->str_val);
        generate_expr(node->left);
        printf(";\n");
    }
    else if (node->type == NODE_PRINT) {
        print_indent(indent_level);
        if (node->left->type == NODE_STR) {
            printf("printf(\"%%s\\n\", %s);\n", node->left->str_val);
        } else if (node->left->type == NODE_ID) {
            printf("printf(\"%%d\\n\", %s);\n", node->left->str_val);
        } else {
            printf("printf(\"%%d\\n\", ");
            generate_expr(node->left);
            printf(");\n");
        }
    }
    else if (node->type == NODE_INPUT) {
        print_indent(indent_level);
        printf("scanf(\"%%d\", &%s);\n", node->str_val);
    }
    else if (node->type == NODE_IF) {
        print_indent(indent_level);
        printf("if (");
        generate_expr(node->cond);
        printf(") {\n");
        
        ASTNode* stmt = node->then_branch;
        while (stmt) {
            generate_code(stmt, indent_level + 1);
            stmt = stmt->next;
        }
        
        print_indent(indent_level);
        printf("}\n");
        if (node->else_branch) {
            print_indent(indent_level);
            printf("else {\n");
            
            stmt = node->else_branch;
            while (stmt) {
                generate_code(stmt, indent_level + 1);
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
            generate_code(stmt, indent_level + 1);
            stmt = stmt->next;
        }
        
        print_indent(indent_level);
        printf("}\n");
    }
}

#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->int_val = 0;
    node->str_val = NULL;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    node->cond = NULL;
    node->then_branch = NULL;
    node->else_branch = NULL;
    return node;
}

ASTNode* create_num_node(int val) {
    ASTNode* node = create_node(NODE_NUM);
    node->int_val = val;
    return node;
}

ASTNode* create_id_node(const char* name) {
    ASTNode* node = create_node(NODE_ID);
    node->str_val = strdup(name);
    return node;
}

ASTNode* create_str_node(const char* str) {
    ASTNode* node = create_node(NODE_STR);
    node->str_val = strdup(str);
    return node;
}

ASTNode* create_binop_node(const char* op, ASTNode* left, ASTNode* right) {
    ASTNode* node = create_node(NODE_BINOP);
    node->str_val = strdup(op);
    node->left = left;
    node->right = right;
    return node;
}

ASTNode* create_var_decl_node(const char* name) {
    ASTNode* node = create_node(NODE_VAR_DECL);
    node->str_val = strdup(name);
    return node;
}

ASTNode* create_assign_node(const char* name, ASTNode* expr) {
    ASTNode* node = create_node(NODE_ASSIGN);
    node->str_val = strdup(name);
    node->left = expr;
    return node;
}

ASTNode* create_print_node(ASTNode* expr) {
    ASTNode* node = create_node(NODE_PRINT);
    node->left = expr;
    return node;
}

ASTNode* create_input_node(const char* name) {
    ASTNode* node = create_node(NODE_INPUT);
    node->str_val = strdup(name);
    return node;
}

ASTNode* create_if_node(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch) {
    ASTNode* node = create_node(NODE_IF);
    node->cond = cond;
    node->then_branch = then_branch;
    node->else_branch = else_branch;
    return node;
}

ASTNode* create_while_node(ASTNode* cond, ASTNode* body) {
    ASTNode* node = create_node(NODE_WHILE);
    node->cond = cond;
    node->then_branch = body;
    return node;
}

ASTNode* create_program_node(ASTNode* stmts) {
    ASTNode* node = create_node(NODE_PROGRAM);
    node->left = stmts;
    return node;
}

ASTNode* create_block_node(ASTNode* stmts) {
    ASTNode* node = create_node(NODE_BLOCK);
    node->left = stmts;
    return node;
}

ASTNode* append_stmt(ASTNode* list, ASTNode* stmt) {
    if (list == NULL) return stmt;
    ASTNode* current = list;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = stmt;
    return list;
}

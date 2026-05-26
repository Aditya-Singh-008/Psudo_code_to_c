#include "ast.h"
#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type       = type;
    node->int_val    = 0;
    node->str_val    = NULL;
    node->values     = NULL;
    node->values_len = 0;
    node->left       = NULL;
    node->right      = NULL;
    node->next       = NULL;
    node->cond       = NULL;
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

/* VAR arr AS INT ARRAY SIZE n
   str_val = array name,  int_val = declared size */
ASTNode* create_array_decl_node(const char* name, int size) {
    ASTNode* node = create_node(NODE_ARRAY_DECL);
    node->str_val = strdup(name);
    node->int_val = size;
    return node;
}

/* VAR arr AS INT ARRAY SIZE n WITH VALUES v1, v2, ...
   str_val = name, int_val = size, values[] = literal list, values_len = count */
ASTNode* create_array_decl_init_node(const char* name, int size, int* vals, int vals_len) {
    ASTNode* node = create_node(NODE_ARRAY_DECL_INIT);
    node->str_val    = strdup(name);
    node->int_val    = size;
    node->values     = vals;      /* caller transfers ownership */
    node->values_len = vals_len;
    return node;
}

/* VAR arr AS INT DYNAMIC ARRAY SIZE n
   C   → int* arr = (int*)malloc(n * sizeof(int));
   Java → ArrayList<Integer> arr = new ArrayList<>(n);
   Py  → arr = []
   str_val = name,  int_val = size hint (0 = unbounded) */
ASTNode* create_array_decl_dynamic_node(const char* name, int size) {
    ASTNode* node = create_node(NODE_ARRAY_DECL_DYNAMIC);
    node->str_val = strdup(name);
    node->int_val = size;
    return node;
}


/* SET arr[index] TO value
   str_val = array name,  left = index expr,  right = value expr */
ASTNode* create_array_assign_node(const char* name, ASTNode* index, ASTNode* value) {
    ASTNode* node = create_node(NODE_ARRAY_ASSIGN);
    node->str_val = strdup(name);
    node->left    = index;
    node->right   = value;
    return node;
}

/* arr[index]  used as rvalue in any expression
   str_val = array name,  left = index expr */
ASTNode* create_array_access_node(const char* name, ASTNode* index) {
    ASTNode* node = create_node(NODE_ARRAY_ACCESS);
    node->str_val = strdup(name);
    node->left    = index;
    return node;
}

/* GET arr[index]  →  scanf("%d", &arr[index]);
   str_val = array name,  left = index expr */
ASTNode* create_array_input_node(const char* name, ASTNode* index) {
    ASTNode* node = create_node(NODE_ARRAY_INPUT);
    node->str_val = strdup(name);
    node->left    = index;
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

/* Recursively frees every node in the AST and all strdup'd strings. */
void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);
    free_ast(node->cond);
    free_ast(node->then_branch);
    free_ast(node->else_branch);
    if (node->str_val) free(node->str_val);
    if (node->values)  free(node->values);
    free(node);
}

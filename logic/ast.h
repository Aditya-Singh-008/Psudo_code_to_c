#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_ARRAY_DECL,    /* VAR arr AS INT ARRAY SIZE n  */
    NODE_ASSIGN,
    NODE_ARRAY_ASSIGN,  /* SET arr[index] TO expr       */
    NODE_ARRAY_ACCESS,  /* arr[index]  (rvalue)         */
    NODE_ARRAY_INPUT,   /* GET arr[index]               */
    NODE_PRINT,
    NODE_INPUT,
    NODE_IF,
    NODE_WHILE,
    NODE_BINOP,
    NODE_NUM,
    NODE_ID,
    NODE_STR
} NodeType;

typedef struct ASTNode {
    NodeType type;

    int int_val;        /* NUMBER value  /  array size  */
    char* str_val;      /* name / operator string       */

    struct ASTNode* left;        /* general child / index expr   */
    struct ASTNode* right;       /* general child / value expr   */
    struct ASTNode* next;        /* sibling in statement list    */
    struct ASTNode* cond;        /* IF/WHILE condition           */
    struct ASTNode* then_branch;
    struct ASTNode* else_branch;
} ASTNode;

ASTNode* create_node(NodeType type);
ASTNode* create_num_node(int val);
ASTNode* create_id_node(const char* name);
ASTNode* create_str_node(const char* str);
ASTNode* create_binop_node(const char* op, ASTNode* left, ASTNode* right);
ASTNode* create_var_decl_node(const char* name);
ASTNode* create_array_decl_node(const char* name, int size);
ASTNode* create_assign_node(const char* name, ASTNode* expr);
ASTNode* create_array_assign_node(const char* name, ASTNode* index, ASTNode* value);
ASTNode* create_array_access_node(const char* name, ASTNode* index);
ASTNode* create_array_input_node(const char* name, ASTNode* index);
ASTNode* create_print_node(ASTNode* expr);
ASTNode* create_input_node(const char* name);
ASTNode* create_if_node(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* cond, ASTNode* body);
ASTNode* create_program_node(ASTNode* stmts);
ASTNode* create_block_node(ASTNode* stmts);
ASTNode* append_stmt(ASTNode* list, ASTNode* stmt);

/* Recursively frees all nodes in the AST to prevent memory leaks. */
void free_ast(ASTNode* node);

#endif

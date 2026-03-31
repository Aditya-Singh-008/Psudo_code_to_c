#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_VAR_DECL,
    NODE_ASSIGN,
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
    
    int int_val;
    char* str_val;
    
    struct ASTNode* left;
    struct ASTNode* right;
    struct ASTNode* next;
    struct ASTNode* cond;
    struct ASTNode* then_branch;
    struct ASTNode* else_branch;
} ASTNode;

ASTNode* create_node(NodeType type);
ASTNode* create_num_node(int val);
ASTNode* create_id_node(const char* name);
ASTNode* create_str_node(const char* str);
ASTNode* create_binop_node(const char* op, ASTNode* left, ASTNode* right);
ASTNode* create_var_decl_node(const char* name);
ASTNode* create_assign_node(const char* name, ASTNode* expr);
ASTNode* create_print_node(ASTNode* expr);
ASTNode* create_input_node(const char* name);
ASTNode* create_if_node(ASTNode* cond, ASTNode* then_branch, ASTNode* else_branch);
ASTNode* create_while_node(ASTNode* cond, ASTNode* body);
ASTNode* create_program_node(ASTNode* stmts);
ASTNode* create_block_node(ASTNode* stmts);
ASTNode* append_stmt(ASTNode* list, ASTNode* stmt);

#endif

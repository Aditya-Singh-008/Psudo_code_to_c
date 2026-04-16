#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"

void generate_code_c(ASTNode* node, int indent_level);
void generate_code_python(ASTNode* node, int indent_level);
void generate_code_java(ASTNode* node, int indent_level);

#endif

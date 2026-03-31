%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symtab.h"

void yyerror(const char *s);
int yylex();

extern ASTNode* ast_root; // Defined in main.c

%}

%union {
    int num;
    char *id;
    struct ASTNode* node;
}

%token START STOP VAR AS INT SET TO SHOW GET
%token IF ELSE ENDIF WHILE DONE
%token PLUS MINUS MULT DIV
%token EQ NEQ LT GT LE GE
%token <num> NUMBER
%token <id> IDENTIFIER STRING

%type <node> program statements statement declaration assignment print_stmt input_stmt if_stmt while_stmt expression condition

%left PLUS MINUS
%left MULT DIV

%%

program:
    START statements STOP { 
        ast_root = create_program_node($2); 
    }
    ;

statements:
    statements statement { $$ = append_stmt($1, $2); }
    | /* empty */ { $$ = NULL; }
    ;

statement:
    declaration { $$ = $1; }
    | assignment { $$ = $1; }
    | print_stmt { $$ = $1; }
    | input_stmt { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    ;

declaration:
    VAR IDENTIFIER AS INT { 
        add_symbol($2, "int");
        $$ = create_var_decl_node($2); 
    }
    ;

assignment:
    SET IDENTIFIER TO expression { 
        $$ = create_assign_node($2, $4); 
    }
    ;

if_stmt:
    IF condition statements ELSE statements ENDIF {
        $$ = create_if_node($2, create_block_node($3), create_block_node($5));
    }
    | IF condition statements ENDIF {
        $$ = create_if_node($2, create_block_node($3), NULL);
    }
    ;

while_stmt:
    WHILE condition statements DONE {
        $$ = create_while_node($2, create_block_node($3));
    }
    ;

expression:
    NUMBER { 
        $$ = create_num_node($1); 
    }
    | IDENTIFIER { 
        $$ = create_id_node($1); 
    }
    | expression PLUS expression  { $$ = create_binop_node("+", $1, $3); }
    | expression MINUS expression { $$ = create_binop_node("-", $1, $3); }
    | expression MULT expression  { $$ = create_binop_node("*", $1, $3); }
    | expression DIV expression   { $$ = create_binop_node("/", $1, $3); }
    | '(' expression ')' {
        $$ = $2;
    }
    ;

condition:
    expression EQ expression  { $$ = create_binop_node("==", $1, $3); }
    | expression NEQ expression { $$ = create_binop_node("!=", $1, $3); }
    | expression LT expression  { $$ = create_binop_node("<", $1, $3); }
    | expression GT expression  { $$ = create_binop_node(">", $1, $3); }
    | expression LE expression  { $$ = create_binop_node("<=", $1, $3); }
    | expression GE expression  { $$ = create_binop_node(">=", $1, $3); }
    ;

print_stmt:
    SHOW STRING { $$ = create_print_node(create_str_node($2)); }
    | SHOW expression { $$ = create_print_node($2); }
    ;

input_stmt:
    GET IDENTIFIER {
        $$ = create_input_node($2);
    }
    ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Translation Error: %s\n", s);
}
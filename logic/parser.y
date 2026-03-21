%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void yyerror(const char *s);
int yylex();

/* Symbol Table */
struct Symbol { char *name; char *type; };
struct Symbol symbol_table[100];
int symbol_count = 0;

void add_symbol(char *name, char *type) {
    symbol_table[symbol_count].name = strdup(name);
    symbol_table[symbol_count].type = strdup(type);
    symbol_count++;
}

char* get_type(char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) return symbol_table[i].type;
    }
    return NULL;
}

/* Helper to combine strings for expressions */
char* combine(char* left, char* op, char* right) {
    char* buf = malloc(512);
    sprintf(buf, "%s %s %s", left, op, right);
    return buf;
}
%}

%union {
    int num;
    char *id;
    char *code; /* New: For passing code strings up the tree */
}

%token START STOP VAR AS INT SET TO SHOW GET
%token PLUS MINUS MULT DIV
%token <num> NUMBER
%token <id> IDENTIFIER STRING
%type <code> expression  /* Tells Bison 'expression' returns a string */

%left PLUS MINUS
%left MULT DIV

%%

program:
    START { printf("#include <stdio.h>\n\nint main() {\n"); } 
    statements 
    STOP { printf("    return 0;\n}\n"); }
    ;

statements:
    statements statement | /* empty */
    ;

statement:
    declaration | assignment | print_stmt | input_stmt
    ;

declaration:
    VAR IDENTIFIER AS INT { 
        add_symbol($2, "int");
        printf("    int %s;\n", $2); 
    }
    ;

/* Clean rule: No actions in the middle. Prints at the end. */
assignment:
    SET IDENTIFIER TO expression { 
        printf("    %s = %s;\n", $2, $4); 
    }
    ;

expression:
    NUMBER { 
        char* buf = malloc(32);
        sprintf(buf, "%d", $1);
        $$ = buf; 
    }
    | IDENTIFIER { 
        $$ = strdup($1); 
    }
    | expression PLUS expression  { $$ = combine($1, "+", $3); }
    | expression MINUS expression { $$ = combine($1, "-", $3); }
    | expression MULT expression  { $$ = combine($1, "*", $3); }
    | expression DIV expression   { $$ = combine($1, "/", $3); }
    | '(' expression ')' {
        char* buf = malloc(512);
        sprintf(buf, "(%s)", $2);
        $$ = buf;
    }
    ;

print_stmt:
    SHOW STRING { printf("    printf(%%s\\n, %s);\n", $2); }
    | SHOW IDENTIFIER {
        char *type = get_type($2);
        if (type && strcmp(type, "int") == 0) 
            printf("    printf(\"%%d\\n\", %s);\n", $2);
    }
    ;

input_stmt:
    GET IDENTIFIER {
        char *type = get_type($2);
        if (type && strcmp(type, "int") == 0) 
            printf("    scanf(\"%%d\", &%s);\n", $2);
    }
    ;

%%

int main() {
    yyparse();
    return 0;
}

void yyerror(const char *s) {
    fprintf(stderr, "Translation Error: %s\n", s);
}
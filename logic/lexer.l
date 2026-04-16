%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>
%}

%%

"START"         { return START; }
"STOP"          { return STOP; }
"VAR"           { return VAR; }
"AS"            { return AS; }
"INT"           { return INT; }
"ARRAY"         { return ARRAY; }
"SIZE"          { return SIZE; }
"SET"           { return SET; }
"TO"            { return TO; }
"SHOW"          { return SHOW; }
"GET"           { return GET; }
"IF"            { return IF; }
"ELSE"          { return ELSE; }
"ENDIF"         { return ENDIF; }
"WHILE"         { return WHILE; }
"DONE"          { return DONE; }

"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LE; }
">="            { return GE; }
"<"             { return LT; }
">"             { return GT; }

[0-9]+          { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z]+       { yylval.id = strdup(yytext); return IDENTIFIER; }
\"[^\"]*\"      { yylval.id = strdup(yytext); return STRING; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }

[ \t\n\r]         ; 
.               { return yytext[0]; }

%%

int yywrap() { return 1; }
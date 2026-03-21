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
"SET"           { return SET; }
"TO"            { return TO; }
"SHOW"          { return SHOW; }
"GET"           { return GET; }

[0-9]+          { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z]+       { yylval.id = strdup(yytext); return IDENTIFIER; }
\"[^\"]*\"      { yylval.id = strdup(yytext); return STRING; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MULT; }
"/"             { return DIV; }

[ \t\n\r]         ; 
.               { return yytext[0]; }

%%

int yywrap() { return 1; }
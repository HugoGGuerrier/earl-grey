%{

#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "parser.tab.h"

%}

binop [+\-\/*%><]|\|\||&&|==|>=|<=
unop [!\-]

integer [0-9]+
ident [a-zA-Z_][a-zA-Z0-9_]*
string "[!-~]*"

%option yylineno

%%

[ \t] ;
[\r\n]      { return(NEW_LINE); }

\(          { return(L_PAREN); }
\)          { return(R_PAREN); }
\[          { return(L_BRACK); }
\]          { return(R_BRACK); }
\{          { return(L_CURLB); }
\}          { return(R_CURLB); }

\;          { return(SEMICOL); }
\:          { return(COL); }
\,          { return(COMMA); }

\=          { return(EQUAL); }

function    { return(FUNCTION_WORD); }
let         { return(LET_WORD); }
if          { return(IF_WORD); }
else        { return(ELSE_WORD); }
while       { return(WHILE_WORD); }
for         { return(FOR_WORD); }
return      { return(RETURN_WORD); }
lambda      { return(LAMBDA_WORD); }

{binop}     { yylval.operator = strdup(yytext); return(BINOP); }
{unop}      { yylval.operator = strdup(yytext); return(UNOP); }

{integer}   { yylval.integer = atoi(yytext); return(INTEGER); }
{ident}     { yylval.string = strdup(yytext); return(IDENT); }
{string}    { yylval.string = strdup(yytext); return(STRING); }

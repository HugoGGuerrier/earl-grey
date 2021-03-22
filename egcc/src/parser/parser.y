%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ast.h"

extern int yylineno;

int yylex (void);
void yyerror (AST_Prog *, const char *);

%}

%token<operator>    BINOP
%token<operator>    UNOP
%token<integer>     INTEGER
%token<string>      IDENT
%token<string>      STRING

%token              L_PAREN R_PAREN L_BRACK R_BRACK L_CURLB R_CURLB
%token              SEMICOL COL COMMA
%token              EQUAL
%token              FUNCTION_WORD VAR_WORD LET_WORD IF_WORD ELSE_WORD WHILE_WORD FOR_WORD RETURN_WORD LAMBDA_WORD
%token              NEW_LINE

%union {
  int integer;
  char *string;
  char *operator;

  AST_Prog prog;

  AST_Stmt stmt;
  AST_Stmts stmts;

  AST_Expr expr;

  AST_Lambda lambda;

  AST_Args args;
  AST_Params params;
}

%type<prog> prog

%type<stmt> stmt
%type<stmts> stmts

%type<expr> expr

%type<lambda> lambda

%type<args> args;
%type<params> params;

%start prog

%locations
%define parse.error verbose
%parse-param {AST_Prog *program_result}
%%

prog: stmts { *program_result = new_prog($1); };

stmt:
  VAR_WORD IDENT EQUAL expr                                                               { $$ = new_var_stmt($2, $4); }
| LET_WORD IDENT EQUAL expr                                                               { $$ = new_let_stmt($2, $4); }
| IDENT EQUAL expr                                                                        { $$ = new_affect_stmt($1, $3); }
| FUNCTION_WORD IDENT L_PAREN params R_PAREN L_CURLB stmts R_CURLB                        { $$ = new_fun_stmt($2, $4, $7); }
| IF_WORD L_PAREN expr R_PAREN L_CURLB stmts R_CURLB ELSE_WORD L_CURLB stmts R_CURLB      { $$ = new_if_stmt($3, $6, $10); }
| IF_WORD L_PAREN expr R_PAREN L_CURLB stmts R_CURLB                                      { $$ = new_if_stmt($3, $6, NULL); }
| WHILE_WORD L_PAREN expr R_PAREN L_CURLB stmts R_CURLB                                   { $$ = new_while_stmt($3, $6); }
| FOR_WORD L_PAREN stmt SEMICOL expr SEMICOL stmt R_PAREN L_CURLB stmts R_CURLB           { $$ = new_for_stmt($3, $5, $7, $10); }
| RETURN_WORD expr                                                                        { $$ = new_return_stmt($2); }
| { $$ = NULL; }
;

stmts:
  stmt                    { $$ = add_stmt(NULL, $1); }
| stmt SEMICOL stmts      { $$ = add_stmt($3, $1); }
| stmt NEW_LINE stmts     { $$ = add_stmt($3, $1); }
;

expr:
  INTEGER                       { $$ = new_int_expr($1); }
| STRING                        { $$ = new_string_expr($1); }
| IDENT                         { $$ = new_ident_expr($1); }
| L_PAREN expr R_PAREN          { $$ = new_paren_expr($2); }
| expr BINOP expr               { $$ = new_binop_expr($1, $2, $3); }
| UNOP expr                     { $$ = new_unop_expr($1, $2); }
| expr L_PAREN args R_PAREN     { $$ = new_app_expr($1, $3); }
| lambda                        { $$ = new_lambda_expr($1); }
;

lambda:
  LAMBDA_WORD L_PAREN params R_PAREN L_CURLB stmts R_CURLB      { $$ = new_lambda($3, $6); }
;

args:
  expr                { $$ = add_arg(NULL, $1); }
| expr COMMA args     { $$ = add_arg($3, $1); }
| { $$ = add_arg(NULL, NULL); }
;

params:
  IDENT                   { $$ = add_param(NULL, $1); }
| IDENT COMMA params      { $$ = add_param($3, $1); }
| { $$ = add_param(NULL, NULL); }
;

%%

void yyerror(AST_Prog *prog, const char *str) {
    fprintf(stderr,"Line: %d\nError : %s\n", yylineno, str);
}

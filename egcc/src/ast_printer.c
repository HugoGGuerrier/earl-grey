#include <stdio.h>

#include "ast_printer.h"
#include "ast.h"


// ===== Internal function declarations =====

static void _indent();

static void _print_prog(AST_Prog prog);
static void _print_stmt(AST_Stmt stmt);
static void _print_stmts(AST_Stmts stmts);
static void _print_expr(AST_Expr expr);
static void _print_lambda(AST_Lambda lambda);
static void _print_args(AST_Args args);
static void _print_params(AST_Params params);
static void _print_binop(AST_Binop binop);
static void _print_unop(AST_Unop unop);


// ===== Global variables =====

static unsigned int indent_level;


// ===== Functions to print the AST =====

// --- Print the indent level
static void _indent() {
    for(unsigned int i = 0 ; i < indent_level ; i++) {
        printf("    ");
    }
}

// --- Print a program
static void _print_prog(AST_Prog prog) {
    _print_stmts(prog->stmts);
}

// --- Print a statement
static void _print_stmt(AST_Stmt stmt) {
    switch (stmt->stmt_type) {

    case LET_STMT:
        _indent();
        printf("let(%s, ", stmt->content.let_stmt.ident);
        _print_expr(stmt->content.let_stmt.expr);
        printf(")");

        break;

    case AFFECT_STMT:
        _indent();
        printf("affect(%s, ", stmt->content.affect_stmt.ident);
        _print_expr(stmt->content.affect_stmt.expr);
        printf(")");

        break;


    case FUN_STMT:
        _indent();
        printf("function(%s, ", stmt->content.fun_stmt.ident);
        printf("(");
        _print_params(stmt->content.fun_stmt.params);
        printf(")) {\n");

        _indent();
        indent_level++;
        _print_stmts(stmt->content.fun_stmt.body);
        indent_level--;
        _indent();
        printf("}");

        break;

    case IF_STMT:
        _indent();
        printf("if(");
        _print_expr(stmt->content.if_stmt.cond);
        printf(") ");

        printf("then {\n");
        indent_level++;
        _print_stmts(stmt->content.if_stmt.conseq);
        indent_level--;
        _indent();
        printf("} ");

        if(stmt->content.if_stmt.altern != NULL) {
            printf("else {\n");
            indent_level++;
            _print_stmts(stmt->content.if_stmt.altern);
            indent_level--;
            _indent();
            printf("}");
        }

        break;

    case WHILE_STMT:
        _indent();
        printf("while(");
        _print_expr(stmt->content.while_stmt.cond);
        printf(")");

        _indent();
        printf(" do {\n");
        indent_level++;
        _print_stmts(stmt->content.while_stmt.body);
        indent_level--;
        _indent();
        printf("}");

        break;

    case FOR_STMT:
        _indent();
        printf("for(");
        int save = indent_level;
        indent_level = 0;
        _print_stmt(stmt->content.for_stmt.init);
        printf(", ");
        _print_expr(stmt->content.for_stmt.cond);
        printf(", ");
        _print_stmt(stmt->content.for_stmt.update);
        indent_level = save;
        printf(")");

        _indent();
        printf(" do {\n");
        indent_level++;
        _print_stmts(stmt->content.for_stmt.body);
        indent_level--;
        _indent();
        printf("}");

        break;

    case RETURN_STMT:
        _indent();
        printf("return(");
        _print_expr(stmt->content.return_stmt);
        printf(")");

        break;
    
    default:
        _indent();
        printf("Unknown statement");
        break;

    }
}

// --- Print many statements
static void _print_stmts(AST_Stmts stmts) {
    if(stmts->head != NULL) {
        _print_stmt(stmts->head);
        printf("\n");
    }

    if(stmts->tail != NULL) {
        _print_stmts(stmts->tail);
    }
}

// --- Print an expression
static void _print_expr(AST_Expr expr) {

    switch (expr->expr_type) {

    case INT_EXPR:
        printf("%d", expr->content.int_expr);
        break;

    case STRING_EXPR:
        printf("%s", expr->content.string_expr);
        break;

    case IDENT_EXPR:
        printf("%s", expr->content.ident_expr);
        break;

    case PAREN_EXPR:
        printf("(");
        _print_expr(expr->content.paren_expr);
        printf(")");
        break;

    case BINOP_EXPR:
        _print_binop(expr->content.binop_expr);
        break;

    case UNOP_EXPR:
        _print_unop(expr->content.unop_expr);
        break;

    case APP_EXPR:
        _print_expr(expr->content.app_expr.expr);
        printf("(");
        _print_args(expr->content.app_expr.args);
        printf(")");
        break;

    case LAMBDA_EXPR:
        _print_lambda(expr->content.lambda_expr);
        break;
    
    default:
        printf("(Unknow expression)");
        break;

    }

}

// --- Print a lambda
static void _print_lambda(AST_Lambda lambda) {
    printf("lambda((");
    _print_params(lambda->params);
    printf(") {\n");
    indent_level++;
    _print_stmts(lambda->body);
    indent_level--;
    _indent();
    printf("}");
}

// --- Print arguments
static void _print_args(AST_Args args) {
    if(args->head != NULL) {
        _print_expr(args->head);
    }

    if(args->tail != NULL) {
        printf(", ");
        _print_args(args->tail);
    }
}

// --- Print parameters
static void _print_params(AST_Params params) {
    if (params->head != NULL) {
        printf("%s", params->head);
    }

    if (params->tail != NULL) {
        printf(", ");
        _print_params(params->tail);
    }
}

// --- Print a binary operation
static void _print_binop(AST_Binop binop) {

    _print_expr(binop->left);

    switch (binop->binop_type) {

    case PLUS:
        printf("+");
        break;

    case MINUS:
        printf("-");
        break;

    case TIMES:
        printf("*");
        break;

    case DIVIDE:
        printf("/");
        break;

    case PERCENT:
        printf("%c", 37);
        break;

    case EQEQ:
        printf("==");
        break;

    case LTEQ:
        printf("<=");
        break;

    case GTEQ:
        printf(">=");
        break;

    case LT:
        printf("<");
        break;

    case GT:
        printf(">");
        break;

    case AND:
        printf("&&");
        break;

    case OR:
        printf("||");
        break;
    
    default:
        printf("(Unknown binary operator)");
        break;

    }

    _print_expr(binop->right);

}

// --- Print an unary operation
static void _print_unop(AST_Unop unop) {

    switch (unop->unop_type) {

    case NEGATE:
        printf("-");
        break;
    
    case NOT:
        printf("!");
        break;

    default:
        printf("(Unknown unary operator)");
        break;

    }

    _print_expr(unop->expr);
    
}

// --- Print the full program
void print_ast(AST_Prog prog) {
    // Init the indent level
    indent_level = 0;

    // Print the AST
    _print_prog(prog);
}
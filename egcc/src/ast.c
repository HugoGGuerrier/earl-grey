#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"


// ===== Internal functions for AST =====

static AST_Binop _new_binop(AST_Expr left, char *name, AST_Expr right);
static AST_Unop _new_unop(char *name, AST_Expr expr);

// --- Create a new binary operator
static AST_Binop _new_binop(AST_Expr left, char *name, AST_Expr right) {
    AST_Binop res = (AST_Binop) malloc(sizeof(struct _binop));
    res->binop_type = BIN_UNKNOWN;

    if(strcmp("+", name) == 0) {
        res->binop_type =  PLUS;
    } else
    if(strcmp("-", name) == 0) {
        res->binop_type = MINUS;
    } else
    if(strcmp("*", name) == 0) {
        res->binop_type = TIMES;
    } else
    if(strcmp("/", name) == 0) {
        res->binop_type = DIVIDE;
    } else
    if(strcmp("%", name) == 0) {
        res->binop_type = PERCENT;
    } else
    if(strcmp("==", name) == 0) {
        res->binop_type = EQEQ;
    } else
    if(strcmp("<=", name) == 0) {
        res->binop_type = LTEQ;
    } else
    if(strcmp(">=", name) == 0) {
        res->binop_type = GTEQ;
    } else
    if(strcmp("<", name) == 0) {
        res->binop_type = LT;
    } else
    if(strcmp(">", name) == 0) {
        res->binop_type = GT;
    } else
    if(strcmp("&&", name) == 0) {
        res->binop_type = AND;
    } else
    if(strcmp("||", name) == 0) {
        res->binop_type = OR;
    }

    free(name);

    res->left = left;
    res->right = right;

    return res;
}

// --- Create a new unary operator
static AST_Unop _new_unop(char *name, AST_Expr expr) {
    AST_Unop res = (AST_Unop) malloc(sizeof(struct _unop));
    res->unop_type = UN_UNKNOWN;

    if(strcmp("-", name) == 0) {
        res->unop_type = NEGATE;
    } else
    if(strcmp("!", name) == 0) {
        res->unop_type = NOT;
    }

    free(name);

    res->expr = expr;

    return res;
}


// ===== Functions to create the AST with dynamic memory =====

// --- Create a new program
AST_Prog new_prog(AST_Stmts stmts) {
    AST_Prog res = (AST_Prog) malloc(sizeof(struct _prog));
    res->stmts = stmts;
    return res;
}


// --- Create a new let statement
AST_Stmt new_let_stmt(char *ident, AST_Expr expr) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = LET_STMT;
    res->content.let_stmt.ident = ident;
    res->content.let_stmt.expr = expr;
    return res;
}

// --- Create a new affect statement
AST_Stmt new_affect_stmt(char *ident, AST_Expr expr) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = AFFECT_STMT;
    res->content.affect_stmt.ident = ident;
    res->content.affect_stmt.expr = expr;
    return res;
}

// --- Create a new statement from a function
AST_Stmt new_fun_stmt(char *ident, AST_Params params, AST_Stmts body) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = FUN_STMT;
    res->content.fun_stmt.ident = ident;
    res->content.fun_stmt.params = params;
    res->content.fun_stmt.body = body;
    return res;
}

// --- Create a new if statement
AST_Stmt new_if_stmt(AST_Expr cond, AST_Stmts conseq, AST_Stmts altern) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = IF_STMT;
    res->content.if_stmt.cond = cond;
    res->content.if_stmt.conseq = conseq;
    res->content.if_stmt.altern = altern;
    return res;
}

// --- Create a new while statement
AST_Stmt new_while_stmt(AST_Expr cond, AST_Stmts body) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = WHILE_STMT;
    res->content.while_stmt.cond = cond;
    res->content.while_stmt.body = body;
    return res;
}

// --- Create a new for statement
AST_Stmt new_for_stmt(AST_Stmt init, AST_Expr cond, AST_Stmt update, AST_Stmts body) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = FOR_STMT;
    res->content.for_stmt.init = init;
    res->content.for_stmt.cond = cond;
    res->content.for_stmt.update = update;
    res->content.for_stmt.body = body;
    return res;
}

// --- Create a new return statement
AST_Stmt new_return_stmt(AST_Expr expr) {
    AST_Stmt res = (AST_Stmt) malloc(sizeof(struct _stmt));
    res->stmt_type = RETURN_STMT;
    res->content.return_stmt = expr;
    return res;
}

// --- Add a statement to a statement set
AST_Stmts add_stmt(AST_Stmts stmts, AST_Stmt stmt) {
    AST_Stmts res = (AST_Stmts) malloc(sizeof(struct _stmts));
    res->head = stmt;
    res->tail = stmts;
    return res;
}


// --- Create a new int expression
AST_Expr new_int_expr(int integer) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = INT_EXPR;
    res->content.int_expr = integer;
    return res;
}

// --- Create a new string expression
AST_Expr new_string_expr(char *string) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = STRING_EXPR;
    res->content.string_expr = string;
    return res;
}

// --- Create a new ident expression
AST_Expr new_ident_expr(char *ident) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = IDENT_EXPR;
    res->content.ident_expr = ident;
    return res;
}

// --- Create a new parented expression
AST_Expr new_paren_expr(AST_Expr expr) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = PAREN_EXPR;
    res->content.paren_expr = expr;
    return res;
}

// --- Create a new binop expression
AST_Expr new_binop_expr(AST_Expr left, char *op, AST_Expr right) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = BINOP_EXPR;
    res->content.binop_expr = _new_binop(left, op, right);
    return res;
}

// --- Create a new unop expression
AST_Expr new_unop_expr(char *op, AST_Expr expr) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = UNOP_EXPR;
    res->content.unop_expr = _new_unop(op, expr);
    return res;
}

// --- Create a new application expression
AST_Expr new_app_expr(AST_Expr expr, AST_Args args) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = APP_EXPR;
    res->content.app_expr.expr = expr;
    res->content.app_expr.args = args;
    return res;
}

// --- Create a new lambda expression
AST_Expr new_lambda_expr(AST_Lambda lambda) {
    AST_Expr res = (AST_Expr) malloc(sizeof(struct _expr));
    res->expr_type = LAMBDA_EXPR;
    res->content.lambda_expr = lambda;
    return res;
}


// --- Create a new lambda
AST_Lambda new_lambda(AST_Params params, AST_Stmts body) {
    AST_Lambda res = (AST_Lambda) malloc(sizeof(struct _lambda));
    res->params = params;
    res->body = body;
    return res;
}


// --- Add an arg to an arg set
AST_Args add_arg(AST_Args args, AST_Expr arg) {
    AST_Args res = (AST_Args) malloc(sizeof(struct _args));
    res->head = arg;
    res->tail = args;
    return res;
}


// --- Add a param to a param set
AST_Params add_param(AST_Params params, char *param) {
    AST_Params res = (AST_Params) malloc(sizeof(struct _params));
    res->head = param;
    res->tail = params;
    return res;
}


// ===== Functions to clean the AST =====

// --- Internal function definitions
static void _clean_prog(AST_Prog prog);
static void _clean_stmt(AST_Stmt stmt);
static void _clean_stmts(AST_Stmts stmts);
static void _clean_expr(AST_Expr expr);
static void _clean_lambda(AST_Lambda lambda);
static void _clean_args(AST_Args args);
static void _clean_params(AST_Params params);

// --- Clean a program
static void _clean_prog(AST_Prog prog) {
    _clean_stmts(prog->stmts);
    free(prog);
}

// --- Clean a statement
static void _clean_stmt(AST_Stmt stmt) {
    switch(stmt->stmt_type) {

    case LET_STMT:
        free(stmt->content.let_stmt.ident);
        _clean_expr(stmt->content.let_stmt.expr);
        break;

    case AFFECT_STMT:
        free(stmt->content.affect_stmt.ident);
        _clean_expr(stmt->content.affect_stmt.expr);
        break;

    case FUN_STMT:
        free(stmt->content.fun_stmt.ident);
        _clean_params(stmt->content.fun_stmt.params);
        _clean_stmts(stmt->content.fun_stmt.body);
        break;

    case IF_STMT:
        _clean_expr(stmt->content.if_stmt.cond);
        _clean_stmts(stmt->content.if_stmt.conseq);
        _clean_stmts(stmt->content.if_stmt.altern);
        break;

    case WHILE_STMT:
        _clean_expr(stmt->content.while_stmt.cond);
        _clean_stmts(stmt->content.while_stmt.body);
        break;

    case FOR_STMT:
        _clean_stmt(stmt->content.for_stmt.init);
        _clean_expr(stmt->content.for_stmt.cond);
        _clean_stmt(stmt->content.for_stmt.update);
        _clean_stmts(stmt->content.for_stmt.body);
        break;

    case RETURN_STMT:
        _clean_expr(stmt->content.return_stmt);
        break;

    default:
        fprintf(stderr, "Unknown statement");

    }

    free(stmt);
}

// --- Clean many statements
static void _clean_stmts(AST_Stmts stmts) {
    if(stmts->head != NULL) {
        _clean_stmt(stmts->head);
    }

    if(stmts->tail != NULL) {
        _clean_stmts(stmts->tail);
    }

    free(stmts);
}

// --- Clean an expression
static void _clean_expr(AST_Expr expr) {
    switch (expr->expr_type) {

    case INT_EXPR:
        break;

    case STRING_EXPR:
        free(expr->content.string_expr);
        break;

    case IDENT_EXPR:
        free(expr->content.ident_expr);
        break;

    case PAREN_EXPR:
        _clean_expr(expr->content.paren_expr);
        break;

    case BINOP_EXPR:
        _clean_expr(expr->content.binop_expr->left);
        _clean_expr(expr->content.binop_expr->right);
        break;

    case UNOP_EXPR:
        _clean_expr(expr->content.unop_expr->expr);
        break;

    case APP_EXPR:
        _clean_expr(expr->content.app_expr.expr);
        _clean_args(expr->content.app_expr.args);
        break;

    case LAMBDA_EXPR:
        _clean_lambda(expr->content.lambda_expr);
        break; 

    default:
        fprintf(stderr, "Unknown expression");

    }

    free(expr);
}

// --- Clean a lambda
static void _clean_lambda(AST_Lambda lambda) {
    _clean_params(lambda->params);
    _clean_stmts(lambda->body);
    free(lambda);
}

// --- Clean some arguments
static void _clean_args(AST_Args args) {
    if(args->head != NULL) {
        _clean_expr(args->head);
    }

    if(args->tail != NULL) {
        _clean_args(args->tail);
    }

    free(args);
}

// --- Clean some params
static void _clean_params(AST_Params params) {
    if(params->head != NULL) {
        free(params->head);
    }

    if(params->tail != NULL) {
        _clean_params(params);
    }

    free(params);
}

// --- Clean the memory of the AST
void clean_ast(AST_Prog prog) {
    _clean_prog(prog);
}
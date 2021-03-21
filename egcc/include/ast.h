#ifndef AST_H
#define AST_H


// ===== Structure definitions =====

// --- Types definition to force pointer usage and avoid static AST node
typedef struct _prog *AST_Prog;

typedef struct _stmt *AST_Stmt;
typedef struct _stmts *AST_Stmts;

typedef struct _expr *AST_Expr;

typedef struct _lambda *AST_Lambda;

typedef struct _args *AST_Args;
typedef struct _params *AST_Params;

typedef struct _binop *AST_Binop;
typedef struct _unop *AST_Unop;

// --- Structure that represent a program
struct _prog {
    AST_Stmts stmts;
};

// --- Structure that represent a statement node
struct _stmt {
    enum {VAR_STMT, LET_STMT, AFFECT_STMT, FUN_STMT, IF_STMT, WHILE_STMT, FOR_STMT, RETURN_STMT} stmt_type;
    union {
        struct {
            char *ident;
            AST_Expr expr;
        } var_stmt;

        struct {
            char *ident;
            AST_Expr expr;
        } let_stmt;

        struct {
            char *ident;
            AST_Expr expr;
        } affect_stmt;

        struct {
            char *ident;
            AST_Params params;
            AST_Stmts body;
        } fun_stmt;

        struct {
            AST_Expr cond;
            AST_Stmts conseq;
            AST_Stmts altern;
        } if_stmt;

        struct {
            AST_Expr cond;
            AST_Stmts body;
        } while_stmt;

        struct {
            AST_Stmt init;
            AST_Expr cond;
            AST_Stmt update;
            AST_Stmts body;
        } for_stmt;

        AST_Expr return_stmt;
    } content;
    
};

// --- Structure that represents many statements
struct _stmts {
    AST_Stmt head;
    AST_Stmts tail;
};

// --- Structure that represents an expression node
struct _expr {
    enum {INT_EXPR, STRING_EXPR, IDENT_EXPR, PAREN_EXPR, BINOP_EXPR, UNOP_EXPR, APP_EXPR, LAMBDA_EXPR} expr_type;
    union {
        int int_expr;

        char *string_expr;

        char *ident_expr;

        AST_Expr paren_expr;

        struct {
            AST_Expr left;
            AST_Binop op;
            AST_Expr right;
        } binop_expr;

        struct {
            AST_Unop op;
            AST_Expr expr;
        } unop_expr;

        struct {
            AST_Expr expr;
            AST_Args args;
        } app_expr;

        AST_Lambda lambda_expr;
    } content;
};

// --- Structure that represents a lambda node
struct _lambda {
    AST_Params params;
    AST_Stmts body;
};

// --- Structure that represents an args node
struct _args {
    AST_Expr head;
    AST_Args tail;
};

// --- Structure that represents a params node
struct _params {
    char *head;
    AST_Params tail;
};

// --- Structure that represents a binary operator node
struct _binop {
    enum {
        PLUS,
        BIN_MINUS,
        TIMES,
        DIVIDE,
        PERCENT,
        EQEQ,
        LTEQ,
        GTEQ,
        LT,
        GT,
        AND,
        OR,

        BIN_UNKNOWN
    } binop_type;
};

// --- Structure that represent an unary operator
struct _unop {
    enum {
        UN_MINUS,
        NOT,

        UN_UNKNOWN
    } unop_type;
};


// ===== Exported function definitions =====

AST_Prog new_prog(AST_Stmts stmts);

AST_Stmt new_var_stmt(char *ident, AST_Expr expr);
AST_Stmt new_let_stmt(char *ident, AST_Expr expr);
AST_Stmt new_affect_stmt(char *ident, AST_Expr expr);
AST_Stmt new_fun_stmt(char *ident, AST_Params params, AST_Stmts body);
AST_Stmt new_if_stmt(AST_Expr cond, AST_Stmts conseq, AST_Stmts altern);
AST_Stmt new_while_stmt(AST_Expr cond, AST_Stmts body);
AST_Stmt new_for_stmt(AST_Stmt init, AST_Expr cond, AST_Stmt update, AST_Stmts body);
AST_Stmt new_return_stmt(AST_Expr expr);
AST_Stmts add_stmt(AST_Stmts stmts, AST_Stmt stmt);

AST_Expr new_int_expr(int integer);
AST_Expr new_string_expr(char *string);
AST_Expr new_ident_expr(char *ident);
AST_Expr new_paren_expr(AST_Expr expr);
AST_Expr new_binop_expr(AST_Expr left, AST_Binop op, AST_Expr right);
AST_Expr new_unop_expr(AST_Unop op, AST_Expr expr);
AST_Expr new_app_expr(AST_Expr expr, AST_Args args);
AST_Expr new_lambda_expr(AST_Lambda lambda);

AST_Lambda new_lambda(AST_Params params, AST_Stmts body);

AST_Args add_arg(AST_Args args, AST_Expr arg);

AST_Params add_param(AST_Params params, char *param);

AST_Binop new_binop(char *name);
AST_Unop new_unop(char *name);

void clean_ast(AST_Prog prog);


#endif
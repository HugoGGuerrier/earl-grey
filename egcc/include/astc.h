#ifndef ASTC_H
#define ASTC_H


// ===== Structure definitions =====

// --- Types definition to force pointer usage and avoid static AST node
typedef struct _c_prog *AST_C_Prog;

typedef struct _c_stmt *AST_C_Stmt;
typedef struct _c_stmts *AST_C_Stmts;

typedef struct _c_expr *AST_C_Expr;

typedef struct _c_lambda *AST_C_Lambda;

typedef struct _c_args *AST_C_Args;
typedef struct _c_params *AST_C_Params;

typedef struct _c_binop *AST_C_Binop;
typedef struct _c_unop *AST_C_Unop;

// --- Structure that represent a program
struct _c_prog {
    unsigned int stack_size;
    AST_C_Stmts stmts;
};

// --- Structure that represent a statement node
struct _c_stmt {
    enum {VAR_C_STMT, LET_C_STMT, AFFECT_C_STMT, FUN_C_STMT, IF_C_STMT, WHILE_C_STMT, FOR_C_STMT, RETURN_C_STMT} stmt_type;
    union {
        struct {
            unsigned int address;
            char *ident;
            AST_C_Expr expr;
        } var_c_stmt;

        struct {
            unsigned int address;
            char *ident;
            AST_C_Expr expr;
        } let_c_stmt;

        struct {
            unsigned int address;
            char *ident;
            AST_C_Expr expr;
        } affect_c_stmt;

        struct {
            unsigned int stack_size;
            char *ident;
            AST_C_Params params;
            AST_C_Stmts body;
        } fun_c_stmt;

        struct {
            AST_C_Expr cond;
            AST_C_Stmts conseq;
            AST_C_Stmts altern;
        } if_c_stmt;

        struct {
            AST_C_Expr cond;
            AST_C_Stmts body;
        } while_c_stmt;

        struct {
            AST_C_Stmt init;
            AST_C_Expr cond;
            AST_C_Stmt update;
            AST_C_Stmts body;
        } for_c_stmt;

        AST_C_Expr return_c_stmt;
    } content;
    
};

// --- Structure that represents many statements
struct _stmts {
    AST_C_Stmt head;
    AST_C_Stmts tail;
};

// --- Structure that represents an expression node
struct _expr {
    enum {INT_C_EXPR, BIGINT_C_EXPR, STRING_C_EXPR, IDENT_C_EXPR, PAREN_C_EXPR, BINOP_C_EXPR, UNOP_C_EXPR, APP_C_EXPR, LAMBDA_C_EXPR} expr_type;
    union {
        int int_c_expr;

        unsigned int bigint_c_expr;

        char *string_c_expr;

        unsigned int ident_c_expr;

        AST_C_Expr paren_c_expr;

        AST_C_Binop binop_c_expr;

        AST_C_Unop unop_c_expr;

        struct {
            AST_C_Expr expr;
            AST_C_Args args;
        } app_c_expr;

        AST_C_Lambda lambda_c_expr;
    } content;
};

// --- Structure that represents a lambda node
struct _lambda {
    unsigned int stack_size;
    AST_C_Params params;
    AST_C_Stmts body;
};

// --- Structure that represents an args node
struct _args {
    AST_C_Expr head;
    AST_C_Args tail;
};

// --- Structure that represents a params node
struct _params {
    unsigned int address;
    char *head;
    AST_C_Params tail;
};

// --- Structure that represents a binary operator node
struct _binop {
    enum {
        PLUS,
        MINUS,
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

    AST_C_Expr left;
    AST_C_Expr right;
};

// --- Structure that represent an unary operator
struct _unop {
    enum {
        NEGATE,
        NOT,

        UN_UNKNOWN
    } unop_type;

    AST_C_Expr expr;
};


// ===== Exported function definitions =====

AST_C_Prog new_c_prog(AST_C_Stmts stmts);

AST_C_Stmt new_var_c_stmt(char *ident, AST_C_Expr expr);
AST_C_Stmt new_let_c_stmt(char *ident, AST_C_Expr expr);
AST_C_Stmt new_affect_c_stmt(char *ident, AST_C_Expr expr);
AST_C_Stmt new_fun_c_stmt(char *ident, AST_C_Params params, AST_C_Stmts body);
AST_C_Stmt new_if_c_stmt(AST_C_Expr cond, AST_C_Stmts conseq, AST_C_Stmts altern);
AST_C_Stmt new_while_c_stmt(AST_C_Expr cond, AST_C_Stmts body);
AST_C_Stmt new_for_c_stmt(AST_C_Stmt init, AST_C_Expr cond, AST_C_Stmt update, AST_C_Stmts body);
AST_C_Stmt new_return_c_stmt(AST_C_Expr expr);
AST_C_Stmts add_c_stmt(AST_C_Stmts stmts, AST_C_Stmt stmt);

AST_C_Expr new_int_c_expr(int integer);
AST_C_Expr new_bigint_c_expr(unsigned int bigint);
AST_C_Expr new_string_c_expr(char *string);
AST_C_Expr new_ident_c_expr(char *ident);
AST_C_Expr new_paren_c_expr(AST_C_Expr expr);
AST_C_Expr new_binop_c_expr(AST_C_Expr left, char *op, AST_C_Expr right);
AST_C_Expr new_unop_c_expr(char *op, AST_C_Expr expr);
AST_C_Expr new_app_c_expr(AST_C_Expr expr, AST_C_Args args);
AST_C_Expr new_lambda_c_expr(AST_C_Lambda lambda);

AST_C_Lambda new_c_lambda(AST_C_Params params, AST_C_Stmts body);

AST_C_Args add_c_arg(AST_C_Args args, AST_C_Expr arg);

AST_C_Params add_c_param(AST_C_Params params, char *param);

void clean_c_ast(AST_C_Prog prog);


#endif
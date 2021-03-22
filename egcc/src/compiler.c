#include "compiler.h"
#include "main.h"

#define ACC 0   // Accumulator / Return
#define SA 1    // Stack Adress
#define SP 2    // Stack Pointer
#define TMP1 3  // Temporary 1
#define TMP2 4  // Temporary 2
#define TMP3 5  // Temporary 3
#define ONE 6   // One, contains value 1
#define MO 7    // Minus One, contains value -1


// ===== Internal function declarations =====

static void _push(int registre);
static void _pop();

static void _compile_prog(AST_Prog prog, compiler_data_t *data);
static void _compile_stmt(AST_Stmt stmt, compiler_data_t *data);
static void _compile_stmts(AST_Stmts stmts, compiler_data_t *data);
static void _compile_binop(AST_Binop binop, compiler_data_t *data);


// ===== Functions to write in the file =====


static void _cond_move(compiler_data_t *data, int a, int b, int c) {}
static void _array_index(compiler_data_t *data, int a, int b, int c);
static void _array_update(compiler_data_t *data, int a, int b, int c);

static void _add(compiler_data_t *data, int a, int b, int c) {

}

static void _multiplication(compiler_data_t *data, int a, int b, int c);
static void _division(compiler_data_t *data, int a, int b, int c);
static void _nand(compiler_data_t *data, int a, int b, int c);
static void _halt(compiler_data_t *data);
static void _allocation(compiler_data_t *data, int b, int c);
static void _free(compiler_data_t *data, int c);
static void _output(compiler_data_t *data, int c);
static void _input(compiler_data_t *data, int c);
static void _load_prog(compiler_data_t *data, int b, int c);
static void _ortho(compiler_data_t *data, unsigned int command);


// ===== Functions to compile the AST =====


static void _push(int register_src) {
    // AMEND SA, SP, register_src
    // ADD SP, SP, ONE
}

static void _pop(int register_dst) {
    // ADD SP, SP, MO
    // INDEX register_dst, SA, SP
}


// --- Compile a program
static void _compile_prog(AST_Prog prog, compiler_data_t *data) {
    _compile_stmts(prog->stmts, data);
}


// --- Compile a statement
static void _compile_stmt(AST_Stmt stmt, compiler_data_t *data) {

}


// --- Compile many statements
static void _compile_stmts(AST_Stmts stmts, compiler_data_t *data) {
    if(stmts->head != NULL) {
        _compile_stmt(stmts->head, data);
    }

    if(stmts->tail != NULL) {
        _compile_stmts(stmts->tail, data);
    }
}


// --- Compile an expression
static void _compile_expr(AST_Expr expr, compiler_data_t *data) {

    switch (expr->expr_type) {

    case INT_EXPR:
        
        break;

    default:
        
        break;

    }

}

// --- Compile a binary operation
static void _compile_binop(AST_Binop binop, compiler_data_t *data) {

    switch (binop->binop_type) {

    case PLUS:
        
        break;

    case BIN_MINUS:
        
        break;

    case TIMES:
        
        break;

    case DIVIDE:
        
        break;

    default:
        
        break;

    }

}


// --- Compile the full program
void compile(AST_Prog prog, compiler_data_t *data) {
    // Initialisation des registres spéciaux ONE et MO

    // ORTHO ONE, 1
    // NAND MO, ONE, ONE
    // ADD MO, MO, ONE


    // Lancement de la compilation de l'AST
    _compile_prog(prog, data);
}
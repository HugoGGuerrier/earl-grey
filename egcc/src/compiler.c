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


static void _push(int register_src, compiler_data_t *data);
static void _pop(int register_dst, compiler_data_t *data);

static void _compile_prog(AST_Prog prog, compiler_data_t *data);
static void _compile_stmt(AST_Stmt stmt, compiler_data_t *data);
static void _compile_stmts(AST_Stmts stmts, compiler_data_t *data);
static void _compile_binop(AST_Binop binop, compiler_data_t *data);


// ===== Functions to write in the file =====


static void _write_op(compiler_data_t *data, int opcode, int a, int b, int c) {
    int op = (opcode << 28) | ((a << 6) | (b << 3) | c);
    fwrite(&op, sizeof(int), 1, data->settings->output_file);
}

static void _cond_move(compiler_data_t *data, int a, int b, int c)          { _write_op(data, 0, a, b, c); }
static void _array_index(compiler_data_t *data, int a, int b, int c)        { _write_op(data, 1, a, b, c); }
static void _array_update(compiler_data_t *data, int a, int b, int c)       { _write_op(data, 2, a, b, c); }
static void _add(compiler_data_t *data, int a, int b, int c)                { _write_op(data, 3, a, b, c); }
static void _multiplication(compiler_data_t *data, int a, int b, int c)     { _write_op(data, 4, a, b, c); }
static void _division(compiler_data_t *data, int a, int b, int c)           { _write_op(data, 5, a, b, c); }
static void _nand(compiler_data_t *data, int a, int b, int c)               { _write_op(data, 6, a, b, c); }

static void _halt(compiler_data_t *data)                                    { _write_op(data, 7, 0, 0, 0); }
static void _allocation(compiler_data_t *data, int b, int c)                { _write_op(data, 8, 0, b, c); }
static void _free(compiler_data_t *data, int c)                             { _write_op(data, 9, 0, 0, c); }
static void _output(compiler_data_t *data, int c)                           { _write_op(data, 10, 0, 0, c); }
static void _input(compiler_data_t *data, int c)                            { _write_op(data, 11, 0, 0, c); }
static void _load_prog(compiler_data_t *data, int b, int c)                 { _write_op(data, 12, 0, b, c); }

static void _ortho(compiler_data_t *data, int a, int value) {
    int op = (13 << 28) | ((a << 25) | value);
    fwrite(&op, sizeof(int), 1, data->settings->output_file);
}


// ===== Functions to compile the AST =====


// --- Push a value on the stack
static void _push(int register_src, compiler_data_t *data) {
    // AMEND SA, SP, register_src
    _array_update(data, SA, SP, register_src);
    // ADD SP, SP, ONE
    _add(data, SP, SP, ONE);
}

// --- Pop a value from the stack
static void _pop(int register_dst, compiler_data_t *data) {
    // ADD SP, SP, MO
    _add(data, SP, SP, MO);
    // INDEX register_dst, SA, SP
    _array_index(data, register_dst, SA, SP);
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
        // expr->content.int_expr
        break;

    case BINOP_EXPR:
        // _compile_expr(expr->content.binop_expr.left, data);
        break;

    default:
        
        break;

    }

}

// --- Compile a binary operation
static void _compile_binop(AST_Binop binop, compiler_data_t *data) {

    switch (binop->binop_type) {

    case PLUS:
        //_compile_expr(_, data);
        break;

    case MINUS:
        
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
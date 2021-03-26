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
static void _compile_expr(AST_Expr expr, compiler_data_t *data);
static void _compile_binop(AST_Binop binop, compiler_data_t *data);
static void _compile_unop(AST_Unop unop, compiler_data_t *data);


// ===== Functions to write in the file =====

static void _write_int(compiler_data_t *data, int d) {
    fwrite(&d, sizeof(int), 1, data->settings->output_file);
    data->offset++;
}

static void _write_op(compiler_data_t *data, int opcode, int a, int b, int c) {
    int op = (opcode << 28) | ((a << 6) | (b << 3) | c);
    //
    // TODO : reverse int to match the endianess
    //
    _write_int(data, op);
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
    _write_int(data, op);
}


// ===== Functions to compile the AST =====


// --- Push a value on the stack
static void _push(int register_src, compiler_data_t *data) {
    _array_update(data, SA, SP, register_src);
    _add(data, SP, SP, ONE);
}

// --- Pop a value from the stack
static void _pop(int register_dst, compiler_data_t *data) {
    _add(data, SP, SP, MO);
    _array_index(data, register_dst, SA, SP);
}


// --- Compile a program
static void _compile_prog(AST_Prog prog, compiler_data_t *data) {
    _compile_stmts(prog->stmts, data);
}


// --- Compile a statement
static void _compile_stmt(AST_Stmt stmt, compiler_data_t *data) {

    switch (stmt->stmt_type) {

    case LET_STMT:
        _compile_expr(stmt->content.let_stmt.expr, data);
        break;
    
    case AFFECT_STMT:
        break;

    case FUN_STMT:
        break;

    case IF_STMT:
        // Compilation of the condition expression
        _compile_expr(stmt->content.if_stmt.cond, data);
        int lbl_then = 0;
        int lbl_else = 0;
        int lbl_endif = 0;
        _ortho(data, TMP2, lbl_then);
        _ortho(data, TMP3, lbl_else);
        // We will load the program at line TMP3 (jump TMP3)
        // So we need to put TMP2 in TMP3 if ACC is true (!=0)
        // This way, TMP3 = lbl_then if ACC is true, and else TMP3 = lbl_else
        _cond_move(data, TMP3, TMP2, ACC);
        // Jump/Loading :
        _ortho(data, TMP1, 0);
        _load_prog(data, TMP1, TMP3);
        // --- lbl_then :
        _compile_expr(stmt->content.if_stmt.conseq, data);
        // and we jump at lbl_endif so we avoid the else part
        _ortho(data, TMP1, 0);
        _ortho(data, TMP2, lbl_endif);
        _load_prog(data, TMP1, TMP2);
        // --- lbl_else :
        _compile_expr(stmt->content.if_stmt.altern, data);
        // --- lbl_endif : nothing mre to do
        break;  

    case WHILE_STMT:
        break;

    case FOR_STMT:
        break;

    case RETURN_STMT:
        break;

    default:
        break;
    }

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
        // The integer value is representable with 25 bits :
        if (expr->content.int_expr < 33554432) {
            _ortho(data, ACC, expr->content.int_expr);
        } else {
            _ortho(data, TMP1, 0);
            _ortho(data, ACC, data->offset + 2);
            _load_prog(data, TMP1, ACC);
            _write_int(data, expr->content.int_expr);
            _ortho(data, ACC, data->offset - 1);
            _array_index(data, ACC, TMP1, ACC);
        }
        break;

    case STRING_EXPR:
        break;

    case IDENT_EXPR:
        break;

    case PAREN_EXPR:
        _compile_expr(data, expr->content.paren_expr);
        break;

    case BINOP_EXPR:
        _compile_binop(expr->content.binop_expr, data);
        break;

    case UNOP_EXPR:
        _compile_unop(expr->content.unop_expr, data);
        break;

    case APP_EXPR:
        break;

    case LAMBDA_EXPR:
        break;
    
    default:
        // data->error->error_code = 
        // data->error->error_message = "No such expression";
        // goto error_end;
        break;

    }

}

// --- Compile a lambda
static void _print_lambda(AST_Lambda lambda) { }

// --- Compile arguments
static void _print_args(AST_Args args) { }

// --- Compile parameters
static void _print_params(AST_Params params) { }

// --- Compile a binary operation
static void _compile_binop(AST_Binop binop, compiler_data_t *data) {

    // The left operand (x) will be in TMP1 and the right one (y) will be in ACC :
    _compile_expr(binop->left, data);
    _push(ACC, data);
    _compile_expr(binop->right, data);
    _pop(TMP1, data);

    switch (binop->binop_type) {

    case PLUS:
        _add(data, ACC, ACC, TMP1);
        break;

    case MINUS:
        // x - y = x + (-1 * y)
        _multiplication(data, ACC, ACC, MO);
        _add(data, ACC, TMP1, ACC);
        break;

    case TIMES:
        _multiplication(data, ACC, TMP1, ACC);
        break;

    case DIVIDE:
        _division(data, ACC, TMP1, ACC);
        break;

    case PERCENT:
        // x % y = res
        _division(data, TMP2, TMP1, ACC);       // x / y = n
        _multiplication(data, TMP2, TMP2, ACC); // n * y = x - res
        _multiplication(data, TMP2, TMP2, MO);  // x - (x - res) = res
        _add(data, ACC, TMP1, TMP2);
        break;

    case EQEQ:
        // Algorithme based on the universal logical operator NAND, following Boole's algebra's rules :
        _nand(data, TMP2, TMP1, ACC);   // r = NAND(x, y)
        _nand(data, TMP1, TMP2, TMP1);  // r_x = NAND(r, x)
        _nand(data, ACC, TMP2, ACC);    // r_y = NAND(r, y)
        _nand(data, TMP1, TMP1, ACC);   // not_res = NAND(r_x, r_y)
        // Interpretation of not_res :
        // (not_res = 0) : x == y
        // (not_res = 1) : x != y
        // Initialisation of the result to true (1)
        _ortho(data, ACC, 1);
        // Put it at false (0) if not_res = 1
        _cond_move(data, ACC, 0, TMP1);
        break;

    case LTEQ:
        break;

    case GTEQ:
        break;

    case LT:
        break;

    case GT:
        break;

    case AND:
        // AND(x, y) = NAND(NAND(x, y), NAND(x,y)), following Boole's algebra's rules 
        _nand(data, ACC, TMP1, ACC);
        _nand(data, ACC, ACC, ACC);
        break;

    case OR:
        // AND(x, y) = NAND(NAND(x, x), NAND(y, y)), following Boole's algebra's rules 
        _nand(data, TMP1, TMP1, TMP1);
        _nand(data, ACC, ACC, ACC);
        _nand(data, ACC, TMP1, ACC);
        break;
    
    default:
        break;

    }

}

// --- Compile an unary operation
static void _compile_unop(AST_Unop unop, compiler_data_t *data) {

    _compile_expr(unop->expr, data);

    switch (unop->unop_type) {

    case NEGATE:
        _multiplication(data, ACC, ACC, MO);
        break;
    
    case NOT:
        // NOT(x) = NAND(x, x), following Boole's algebra's rules 
        _nand(data, ACC, ACC, ACC);
        break;

    default:
        break;

    }
    
}

// --- Compile the full program
void compile(AST_Prog prog, compiler_data_t *data) {
    // Initialisation des registres spéciaux ONE = 1 et MO = -1 :
    _ortho(data, ONE, 1);
    _nand(data, MO, ONE, ONE);
    _add(data, MO, MO, ONE);


    // Lancement de la compilation de l'AST
    _compile_prog(prog, data);

    // error_end:
    // return 1;
}
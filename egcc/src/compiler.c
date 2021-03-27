#include "compiler.h"
#include "main.h"

#define ACC 0   // Accumulator / Return register
#define SA 1    // Stack Adress register
#define SP 2    // Stack Pointer register
#define TMP1 3  // Temporary 1 register
#define TMP2 4  // Temporary 2 register
#define TMP3 5  // Temporary 3 register
#define ONE 6   // One register, contains value 1
#define MO 7    // Minus One register, contains value -1


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

/*
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
*/


// ===== Functions to create the labeled instructions array =====

static void _add_lbl_instr(compiler_data_t *data, instruction instr, char *label) {
    // Testing the array's capacity 
    if(data->arr_offset >= data->arr_size) {
        data->arr_size = data->arr_size * 2;
        data->lbl_instr_arr = (labeled_instruction *) realloc(data->lbl_instr_arr, data->arr_size * sizeof(labeled_instruction));
    }
    // Create and add the labeled instruction to the array
    labeled_instruction lbl_instr;
    lbl_instr.instr = &instr;
    lbl_instr.label = label;
    data->lbl_instr_arr[data->arr_offset] = lbl_instr;
    data->arr_offset++;
}

static instruction _create_std_instr(compiler_data_t *data, int opcode, int a, int b, int c) {
    // Construct the new instruction
    instruction instr;
    instr.op_type = STD_OP;
    instr.content.std_op.opcode = opcode;
    instr.content.std_op.a = a;
    instr.content.std_op.b = b;
    instr.content.std_op.c = c;
    return instr;
}

// --- Standard instructions
static void _cond_move(compiler_data_t *data, int a, int b, int c, char *label)          { _add_lbl_instr(data, _create_std_instr(data, 0, a, b, c), label); }
static void _array_index(compiler_data_t *data, int a, int b, int c, char *label)        { _add_lbl_instr(data, _create_std_instr(data, 1, a, b, c), label); }
static void _array_update(compiler_data_t *data, int a, int b, int c, char *label)       { _add_lbl_instr(data, _create_std_instr(data, 2, a, b, c), label); }
static void _add(compiler_data_t *data, int a, int b, int c, char *label)                { _add_lbl_instr(data, _create_std_instr(data, 3, a, b, c), label); }
static void _multiplication(compiler_data_t *data, int a, int b, int c, char *label)     { _add_lbl_instr(data, _create_std_instr(data, 4, a, b, c), label); }
static void _division(compiler_data_t *data, int a, int b, int c, char *label)           { _add_lbl_instr(data, _create_std_instr(data, 5, a, b, c), label); }
static void _nand(compiler_data_t *data, int a, int b, int c, char *label)               { _add_lbl_instr(data, _create_std_instr(data, 6, a, b, c), label); }

static void _halt(compiler_data_t *data, char *label)                                    { _add_lbl_instr(data, _create_std_instr(data, 7, 0, 0, 0), label); }
static void _allocation(compiler_data_t *data, int b, int c, char *label)                { _add_lbl_instr(data, _create_std_instr(data, 8, 0, b, c), label); }
static void _free(compiler_data_t *data, int c, char *label)                             { _add_lbl_instr(data, _create_std_instr(data, 9, 0, 0, c), label); }
static void _output(compiler_data_t *data, int c, char *label)                           { _add_lbl_instr(data, _create_std_instr(data, 10, 0, 0, c), label); }
static void _input(compiler_data_t *data, int c, char *label)                            { _add_lbl_instr(data, _create_std_instr(data, 11, 0, 0, c), label); }
static void _load_prog(compiler_data_t *data, int b, int c, char *label)                 { _add_lbl_instr(data, _create_std_instr(data, 12, 0, b, c), label); }

// --- Special instructions
static void _ortho(compiler_data_t *data, int a, int value, char *label, char *target_label) {
    // Only the ORTHO operators can take as value an int or a target label.
    instruction instr;
    instr.op_type = ORTHO_OP;
    instr.content.ortho_op.opcode = 13;
    instr.content.ortho_op.a = a;
    instr.content.ortho_op.value = value;
    instr.content.ortho_op.target_lbl = target_label;
    _add_lbl_instr(data, instr, label);
}

static void _bigint(compiler_data_t *data, int value, char *label) {
    instruction instr;
    instr.op_type = BIGINT;
    instr.content.big_int = value;
    _add_lbl_instr(data, instr, label);
}

// ===== Functions to compile the AST =====


// --- Push a value on the stack
static void _push(int register_src, compiler_data_t *data) {
    _array_update(data, SA, SP, register_src, NULL);
    _add(data, SP, SP, ONE, NULL);
}

// --- Pop a value from the stack
static void _pop(int register_dst, compiler_data_t *data) {
    _add(data, SP, SP, MO, NULL);
    _array_index(data, register_dst, SA, SP, NULL);
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

        char *lbl_then = "";
        char *lbl_else = "";
        char *lbl_endif = "";

        // Load the then & else labels
        _ortho(data, TMP2, 0, NULL, lbl_then);
        _ortho(data, TMP3, 0, NULL, lbl_else);

        // We will load the program at line TMP3 (jump TMP3)
        // So we need to put TMP2 in TMP3 if ACC is true (!=0)
        // This way, TMP3 = lbl_then if ACC is true, and else TMP3 = lbl_else
        _cond_move(data, TMP3, TMP2, ACC, NULL);
        // Jump/Loading :
        _ortho(data, TMP1, 0, NULL, NULL);
        _load_prog(data, TMP1, TMP3, NULL);

        // --- lbl_then :
        // Labelise
        _ortho(data, TMP1, 0, lbl_then, NULL);
        // Compile if's consequence
        _compile_expr(stmt->content.if_stmt.conseq, data);
        // and we jump at lbl_endif so we avoid the else part
        _ortho(data, TMP1, 0, NULL, NULL);
        _ortho(data, TMP2, 0, NULL, lbl_endif);
        _load_prog(data, TMP1, TMP2, NULL);

        // --- lbl_else :
        // Labelise
        _ortho(data, TMP1, 0, lbl_else, NULL); 
        // Compile if's alternative
        _compile_expr(stmt->content.if_stmt.altern, data);

        // --- lbl_endif : 
        // Nothing more to do, except labelising the next instruction
        _ortho(data, TMP1, 0, lbl_endif, NULL);
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
        // The max integer value for ORTHO instruction should be encodable on 25 bits :
        if (expr->content.int_expr < 33554432) {
            // It is encodable on 25 bits
            _ortho(data, ACC, expr->content.int_expr, NULL, NULL);
        } else {
            // It is not encodable on 25 bits : kind of "bigint", even if is still a 32bits-integer
            char *lbl_bigint = "";
            char *lbl_after_bigint = "";
            // Jump/Load the program after the bigint
            _ortho(data, TMP1, 0, NULL, NULL);
            _ortho(data, ACC, 0, NULL, lbl_after_bigint);
            _load_prog(data, TMP1, ACC, NULL);
            // Labelised bigint
            _bigint(data, expr->content.int_expr, lbl_bigint);
            // After bigint : store the bigint in ACC
            _ortho(data, ACC, 0, lbl_after_bigint, lbl_bigint);
            _array_index(data, ACC, TMP1, ACC, NULL);
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
        _add(data, ACC, ACC, TMP1, NULL);
        break;

    case MINUS:
        // x - y = x + (-1 * y)
        _multiplication(data, ACC, ACC, MO, NULL);
        _add(data, ACC, TMP1, ACC, NULL);
        break;

    case TIMES:
        _multiplication(data, ACC, TMP1, ACC, NULL);
        break;

    case DIVIDE:
        _division(data, ACC, TMP1, ACC, NULL);
        break;

    case PERCENT:
        // x % y = res
        _division(data, TMP2, TMP1, ACC, NULL);       // x / y = n
        _multiplication(data, TMP2, TMP2, ACC, NULL); // n * y = x - res
        _multiplication(data, TMP2, TMP2, MO, NULL);  // x - (x - res) = res
        _add(data, ACC, TMP1, TMP2, NULL);
        break;

    case EQEQ:
        // Algorithme based on the universal logical operator NAND, following Boole's algebra's rules :
        _nand(data, TMP2, TMP1, ACC, NULL);   // r = NAND(x, y)
        _nand(data, TMP1, TMP2, TMP1, NULL);  // r_x = NAND(r, x)
        _nand(data, ACC, TMP2, ACC, NULL);    // r_y = NAND(r, y)
        _nand(data, TMP1, TMP1, ACC, NULL);   // not_res = NAND(r_x, r_y)
        // Interpretation of not_res :
        // (not_res = 0) : x == y
        // (not_res = 1) : x != y
        // Initialisation of the result to true (1)
        _ortho(data, ACC, 1, NULL, NULL);
        // Put it at false (0) if not_res = 1
        _cond_move(data, ACC, 0, TMP1, NULL);
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
        _nand(data, ACC, TMP1, ACC, NULL);
        _nand(data, ACC, ACC, ACC, NULL);
        break;

    case OR:
        // AND(x, y) = NAND(NAND(x, x), NAND(y, y)), following Boole's algebra's rules 
        _nand(data, TMP1, TMP1, TMP1, NULL);
        _nand(data, ACC, ACC, ACC, NULL);
        _nand(data, ACC, TMP1, ACC, NULL);
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
        _multiplication(data, ACC, ACC, MO, NULL);
        break;
    
    case NOT:
        // NOT(x) = NAND(x, x), following Boole's algebra's rules 
        _nand(data, ACC, ACC, ACC, NULL);
        break;

    default:
        break;

    }
    
}

// --- Compile the full program
void compile(AST_Prog prog, compiler_data_t *data) {
    // data->labeled_instruction_arr initialisation :
    data->arr_size = 4;
    data->arr_offset = 0;
    data->lbl_instr_arr = (labeled_instruction *) malloc(data->arr_size * sizeof(labeled_instruction));
    // Special registers initialisations : ONE = 1, MO = -1 :
    _ortho(data, ONE, 1, NULL, NULL);
    _nand(data, MO, ONE, ONE, NULL);
    _add(data, MO, MO, ONE, NULL);

    // Compile the full AST
    _compile_prog(prog, data);

    // Cleaning memory
    free(data->lbl_instr_arr);
    clean_ast(prog);

    // error_end:
    // return 1;
}
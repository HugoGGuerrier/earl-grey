#include "compiler.h"
#include "utils.h"
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
static void _print_lambda(AST_Lambda lambda, compiler_data_t *data);
static void _print_args(AST_Args args, compiler_data_t *data);
static void _print_params(AST_Params params, compiler_data_t *data);
static void _compile_binop(AST_Binop binop, compiler_data_t *data);
static void _compile_unop(AST_Unop unop, compiler_data_t *data);


// ===== Primitives =====


static void _print_int(int x) {
    printf("%d\n", x);
}


// ===== Functions to write in the file =====


static void _write_int(compiler_data_t *data, int x) {
    x = reverse(x); // Reverse the endianess
    fwrite(&x, sizeof(int), 1, data->settings->output_file);
}

static void _write_std_op(compiler_data_t *data, int opcode, int a, int b, int c) {
    int op = (opcode << 28) | ((a << 6) | (b << 3) | c);
    _write_int(data, op);
}

static void _write_ortho_op(compiler_data_t *data, int a, int value) {
    int op = (13 << 28) | ((a << 25) | value);
    _write_int(data, op);
}


// --- Generate bytecode from the labeled instructions array and from the label-adress array
static void _generate_bytecode(compiler_data_t *data) {

    for (int i = 0; i < data->arr_size; i++) {

        instruction *instr = data->lbl_instr_arr[i]->instr;
        
        switch (instr->op_type) {
        
        case STD_OP:
            _write_std_op(data, instr->content.std_op.opcode, instr->content.std_op.a, instr->content.std_op.b, instr->content.std_op.c);
            break;

        case ORTHO_OP:
            // Replace labels
            if (instr->content.ortho_op.val_is_target_lbl) { // val_is_target != 0
                // Replace the label name by its adress value
                instr->content.ortho_op.val = data->lbl_adress_arr[instr->content.ortho_op.val];
            }
            // Send it to the writter
            _write_ortho_op(data, instr->content.ortho_op.a, instr->content.ortho_op.val);
            break;

        case BIGINT:
            // Send it to the writter 
            _write_int(data, instr->content.big_int);
            break;
        
        default:
            break;
        }       

    }
}


// ===== Function to link labels to their adress =====


static void _link_labels(compiler_data_t *data) {
    for (int i = 0; i < data->arr_size; i++) {
        int label = data->lbl_instr_arr[i]->label;
        if (label != -1) {
            // The line is labeled so we store the label and the line number
            data->lbl_adress_arr[label] = i;
        }
    }
    
}


// ===== Functions to create the labeled instructions array =====

// --- Add a labeled instruction (created from the instruction given as a parameter) in the lbl_instr_arr
static void _add_lbl_instr(compiler_data_t *data, instruction *instr, int label) {
    // Testing the array's capacity 
    if(data->arr_offset >= data->arr_size) {
        data->arr_size = data->arr_size * 2;
        data->lbl_instr_arr = (labeled_instruction **) realloc(data->lbl_instr_arr, data->arr_size * sizeof(labeled_instruction *));
    }
    // Create and add the labeled instruction to the array
    labeled_instruction *lbl_instr = (labeled_instruction *) malloc(sizeof(labeled_instruction));
    lbl_instr->instr = instr;
    lbl_instr->label = label;
    data->lbl_instr_arr[data->arr_offset] = lbl_instr;
    data->arr_offset++;
}

// --- Create an instruction from an opcode int, and 3 integers (register numbers) 
static instruction * _create_std_instr(compiler_data_t *data, int opcode, int a, int b, int c) {
    // Construct the new instruction
    instruction *instr = (instruction *) malloc(sizeof(instruction));
    instr->op_type = STD_OP;
    instr->content.std_op.opcode = opcode;
    instr->content.std_op.a = a;
    instr->content.std_op.b = b;
    instr->content.std_op.c = c;
    return instr;
}

// --- Standard instructions
static void _cond_move(compiler_data_t *data, int a, int b, int c, int label)          { _add_lbl_instr(data, _create_std_instr(data, 0, a, b, c), label); }
static void _array_index(compiler_data_t *data, int a, int b, int c, int label)        { _add_lbl_instr(data, _create_std_instr(data, 1, a, b, c), label); }
static void _array_update(compiler_data_t *data, int a, int b, int c, int label)       { _add_lbl_instr(data, _create_std_instr(data, 2, a, b, c), label); }
static void _add(compiler_data_t *data, int a, int b, int c, int label)                { _add_lbl_instr(data, _create_std_instr(data, 3, a, b, c), label); }
static void _multiplication(compiler_data_t *data, int a, int b, int c, int label)     { _add_lbl_instr(data, _create_std_instr(data, 4, a, b, c), label); }
static void _division(compiler_data_t *data, int a, int b, int c, int label)           { _add_lbl_instr(data, _create_std_instr(data, 5, a, b, c), label); }
static void _nand(compiler_data_t *data, int a, int b, int c, int label)               { _add_lbl_instr(data, _create_std_instr(data, 6, a, b, c), label); }
static void _halt(compiler_data_t *data, int label)                                    { _add_lbl_instr(data, _create_std_instr(data, 7, 0, 0, 0), label); }
static void _allocation(compiler_data_t *data, int b, int c, int label)                { _add_lbl_instr(data, _create_std_instr(data, 8, 0, b, c), label); }
static void _free(compiler_data_t *data, int c, int label)                             { _add_lbl_instr(data, _create_std_instr(data, 9, 0, 0, c), label); }
static void _output(compiler_data_t *data, int c, int label)                           { _add_lbl_instr(data, _create_std_instr(data, 10, 0, 0, c), label); }
static void _input(compiler_data_t *data, int c, int label)                            { _add_lbl_instr(data, _create_std_instr(data, 11, 0, 0, c), label); }
static void _load_prog(compiler_data_t *data, int b, int c, int label)                 { _add_lbl_instr(data, _create_std_instr(data, 12, 0, b, c), label); }

// --- Special instructions
static void _ortho(compiler_data_t *data, int a, int value, char val_is_target_lbl, int label) {
    // Only the ORTHO operators can take as value an int or a target label.
    instruction *instr = (instruction *) malloc(sizeof(instruction));
    instr->op_type = ORTHO_OP;
    instr->content.ortho_op.opcode = 13;
    instr->content.ortho_op.a = a;
    instr->content.ortho_op.val = value;
    instr->content.ortho_op.val_is_target_lbl = val_is_target_lbl;
    _add_lbl_instr(data, instr, label);
}

static void _bigint(compiler_data_t *data, int value, int label) {
    instruction *instr = (instruction *) malloc(sizeof(instruction));
    instr->op_type = BIGINT;
    instr->content.big_int = value;
    _add_lbl_instr(data, instr, label);
}


// ===== Functions to compile the AST =====


// --- Push a value on the stack
static void _push(int register_src, compiler_data_t *data) {
    _array_update(data, SA, SP, register_src, -1);
    _add(data, SP, SP, ONE, -1);
}

// --- Pop a value from the stack
static void _pop(int register_dst, compiler_data_t *data) {
    _add(data, SP, SP, MO, -1);
    _array_index(data, register_dst, SA, SP, -1);
}


// --- Compile a program
static void _compile_prog(AST_Prog prog, compiler_data_t *data) {
    _compile_stmts(prog->stmts, data);
}


// --- Compile a statement
static void _compile_stmt(AST_Stmt stmt, compiler_data_t *data) {

    int lbl_x, lbl_y, lbl_z;

    switch (stmt->stmt_type) {

    case LET_STMT:
        _compile_expr(stmt->content.let_stmt.expr, data);
        // TODO
        break;
    
    case AFFECT_STMT:
        _compile_expr(stmt->content.affect_stmt.expr, data);
        // TODO
        break;

    case FUN_STMT:
        // TODO
        break;

    case IF_STMT:
        // Compilation of the condition expression
        _compile_expr(stmt->content.if_stmt.cond, data);

        lbl_x = data->nb_lbl++; // then_lbl
        lbl_y = data->nb_lbl++; // else_lbl
        lbl_z = data->nb_lbl++; // endif_lbl

        // Load the then & else labels
        _ortho(data, TMP2, lbl_x, 1, -1);
        _ortho(data, TMP3, lbl_y, 1, -1);

        // We will load the program at line TMP3 (jump TMP3)
        // So we need to put TMP2 in TMP3 if ACC is true (!=0)
        // This way, TMP3 = lbl_then if ACC is true, and else TMP3 = lbl_else
        _cond_move(data, TMP3, TMP2, ACC, -1);
        // Jump/Loading :
        _ortho(data, TMP1, 0, 0, -1);
        _load_prog(data, TMP1, TMP3, -1);

        // --- lbl_then :
        // Labelise
        _ortho(data, TMP1, 0, 0, lbl_x);
        // Compile if's consequence
        _compile_stmts(stmt->content.if_stmt.conseq, data);
        // and we jump at lbl_endif so we avoid the else part
        _ortho(data, TMP1, 0, 0, -1);
        _ortho(data, TMP2, lbl_z, 1, -1);
        _load_prog(data, TMP1, TMP2, -1);

        // --- lbl_else :
        // Labelise
        _ortho(data, TMP1, 0, 0, lbl_y); 
        // Compile if's alternative
        _compile_stmts(stmt->content.if_stmt.altern, data);

        // --- lbl_endif : 
        // Nothing more to do, except labelising the next instruction
        _ortho(data, TMP1, 0, 0, lbl_z);
        break;

    case WHILE_STMT:
        lbl_x = data->nb_lbl++; // while_cond
        lbl_y = data->nb_lbl++; // while_body
        lbl_z = data->nb_lbl++; // while_end

        // --- lbl_while_cond :
        // Labelise
        _ortho(data, TMP1, 0, 0, lbl_x);
        // Compilation of the condition expression
        _compile_expr(stmt->content.while_stmt.cond, data);
        // Load the body & end labels
        _ortho(data, TMP2, lbl_z, 1, -1);
        _ortho(data, TMP3, lbl_y, 1, -1);
        // Test the result of the condition
        _cond_move(data, TMP2, TMP3, ACC, -1);
        // Jump/Loading
        _ortho(data, TMP1, 0, 0, -1);
        _load_prog(data, TMP1, TMP2, -1);

        // --- lbl_while_body :
        // Labelise
        _ortho(data, TMP1, 0, 0, lbl_y);
        // Compilation of the body expression
        _compile_stmts(stmt->content.while_stmt.body, data);
        // Jump back to the condition
        _ortho(data, TMP1, 0, 0, -1);
        _ortho(data, TMP2, lbl_x, 1, -1);
        _load_prog(data, TMP1, TMP2, -1);

        // --- lbl_while_end :
        // Labelise
        _ortho(data, TMP1, 0, 0, lbl_z);
        break;

    case FOR_STMT:
        lbl_x = data->nb_lbl++; // for_cond
        lbl_y = data->nb_lbl++; // for_body
        lbl_z = data->nb_lbl++; // for end

        // Initialisation
        // TODO

        break;

    case RETURN_STMT:
        _compile_expr(stmt->content.return_stmt, data);
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

    int lbl_x, lbl_y;

    case INT_EXPR:
        // The max integer value for ORTHO instruction should be encodable on 25 bits :
        if (expr->content.int_expr < 33554432) {
            // It is encodable on 25 bits
            _ortho(data, ACC, expr->content.int_expr, 0, -1);
        } else {
            // It is not encodable on 25 bits : kind of "bigint", even if is still a 32bits-integer
            lbl_x = data->nb_lbl++;
            lbl_y = data->nb_lbl++;
            // Jump/Load the program after the bigint
            _ortho(data, TMP1, 0, 0, -1);
            _ortho(data, ACC, lbl_y, 1, -1);
            _load_prog(data, TMP1, ACC, -1);
            // Labelised bigint
            _bigint(data, expr->content.int_expr, lbl_x);
            // After bigint : store the bigint in ACC
            _ortho(data, ACC, lbl_x, 1, lbl_y);
            _array_index(data, ACC, TMP1, ACC, -1);
        }
        break;

    case STRING_EXPR:
        // TODO
        break;

    case IDENT_EXPR:
        // TODO
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
        // TODO
        break;

    case LAMBDA_EXPR:
        // TODO
        break;
    
    default:
        break;

    }

}

// --- Compile a lambda
static void _compile_lambda(AST_Lambda lambda, compiler_data_t *data) { 
    // TODO
}

// --- Compile arguments
static void _compile_args(AST_Args args, compiler_data_t *data) { 
    // TODO
}

// --- Compile parameters
static void _compile_params(AST_Params params, compiler_data_t *data) { 
    // TODO
}

// --- Compile a binary operation
static void _compile_binop(AST_Binop binop, compiler_data_t *data) {

    // The left operand (x) will be in TMP1 and the right one (y) will be in ACC :
    _compile_expr(binop->left, data);
    _push(ACC, data);
    _compile_expr(binop->right, data);
    _pop(TMP1, data);

    switch (binop->binop_type) {

    case PLUS:
        _add(data, ACC, ACC, TMP1, -1);
        break;

    case MINUS:
        // x - y = x + (-1 * y)
        _multiplication(data, ACC, ACC, MO, -1);
        _add(data, ACC, TMP1, ACC, -1);
        break;

    case TIMES:
        _multiplication(data, ACC, TMP1, ACC, -1);
        break;

    case DIVIDE:
        _division(data, ACC, TMP1, ACC, -1);
        break;

    case PERCENT:
        // x % y = res
        _division(data, TMP2, TMP1, ACC, -1);       // x / y = n
        _multiplication(data, TMP2, TMP2, ACC, -1); // n * y = x - res
        _multiplication(data, TMP2, TMP2, MO, -1);  // x - (x - res) = res
        _add(data, ACC, TMP1, TMP2, -1);
        break;

    case EQEQ:
        // Algorithm based on the universal logical operator NAND, following Boole's algebra's rules :
        _nand(data, TMP2, TMP1, ACC, -1);   // r = NAND(x, y)
        _nand(data, TMP1, TMP2, TMP1, -1);  // r_x = NAND(r, x)
        _nand(data, ACC, TMP2, ACC, -1);    // r_y = NAND(r, y)
        _nand(data, TMP1, TMP1, ACC, -1);   // not_res = NAND(r_x, r_y)
        // Interpretation of not_res :
        // (not_res = 0) : x == y
        // (not_res = 1) : x != y
        // Initialisation of the result to true (1)
        _ortho(data, ACC, 1, 0, -1);
        // Put it at false (0) if not_res = 1
        _cond_move(data, ACC, 0, TMP1, -1);
        break;

    case LTEQ:
        // TODO
        break;

    case GTEQ:
        // TODO
        break;

    case LT:
        // If x/y = 0, then x < y
        _division(data, TMP1, TMP1, ACC, -1);
        _ortho(data, ACC, 1, 0, -1);
        _ortho(data, TMP2, 0, 0, -1);
        _cond_move(data, ACC, TMP2, ACC, -1);
        break;

    case GT:
        // If y/x = 0, then x > y
        _division(data, TMP1, ACC, TMP1, -1);
        _ortho(data, ACC, 1, 0, -1);
        _ortho(data, TMP2, 0, 0, -1);
        _cond_move(data, ACC, TMP2, ACC, -1);
        break;

    case AND:
        // AND(x, y) = NAND(NAND(x, y), NAND(x,y)), following Boole's algebra's rules 
        _nand(data, ACC, TMP1, ACC, -1);
        _nand(data, ACC, ACC, ACC, -1);
        break;

    case OR:
        // AND(x, y) = NAND(NAND(x, x), NAND(y, y)), following Boole's algebra's rules 
        _nand(data, TMP1, TMP1, TMP1, -1);
        _nand(data, ACC, ACC, ACC, -1);
        _nand(data, ACC, TMP1, ACC, -1);
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
        _multiplication(data, ACC, ACC, MO, -1);
        break;
    
    case NOT:
        // NOT(x) = NAND(x, x), following Boole's algebra's rules 
        _nand(data, ACC, ACC, ACC, -1);
        break;

    default:
        break;

    }
    
}

// --- Compile the full program
void compile(AST_Prog prog, compiler_data_t *data) {
    // data->... initialisations :
    data->arr_size = 8;
    data->arr_offset = 0;
    data->lbl_instr_arr = (labeled_instruction **) malloc(data->arr_size * sizeof(labeled_instruction *));
    data->nb_lbl = 0;

    // Registers initialisations
    // ONE = 1, MO = -1 :
    _ortho(data, ONE, 1, 0, -1);
    _nand(data, MO, ONE, ONE, -1);
    _add(data, MO, MO, ONE, -1);

    // First pass : Compile the full AST
    _compile_prog(prog, data);

    // Second pass : Link the labels to their adress
    data->lbl_adress_arr = (int *) malloc(data->nb_lbl * sizeof(int));
    _link_labels(data);

    // Third pass : Generate the bytecode with replacement of labels
    _generate_bytecode(data);

    // Cleaning memory
    free(data->lbl_instr_arr);
    clean_ast(prog);

    // error_end:
    // return 1;
}
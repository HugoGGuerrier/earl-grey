#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "main.h"


// ===== Structure definitions =====

// --- Structure to contain all compiler's configuration
typedef struct {
    unsigned int flags;
    char *input_file_name;
    FILE *input_file;
    char *output_file_name;
    FILE *output_file;
    char **include_dirs;
} compiler_settings_t;

// --- Structure to handle compiler errors
typedef struct {
    int error_code;
    const char *error_message;
} compiler_error_t;

// --- Structure to contain an instruction
typedef struct {
    enum {STD_OP, ORTHO_OP, BIGINT} op_type;
    union {
        struct {
            int opcode;
            int a;
            int b;
            int c;
        } std_op;

        struct {
            int opcode;
            int a;
            int value;
            char *target_lbl;
        } ortho_op;

        int big_int;
    } content;
} instruction;

// --- Structure to associate labels to instructions
typedef struct {
    char *label;
    instruction *instr;
} labeled_instruction;

// --- Structure to contain data for the compiler
typedef struct {
    compiler_settings_t *settings;
    compiler_error_t *error;
    labeled_instruction **lbl_instr_arr;
    unsigned int arr_size;
    unsigned int arr_offset;
} compiler_data_t;


// ===== Exported function definitions =====

void compile(AST_Prog prog, compiler_data_t *data);
void raise_error(compiler_error_t *error, int code, const char *message);

#endif
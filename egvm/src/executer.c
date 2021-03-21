#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executer.h"
#include "machine.h"
#include "utils.h"

// ===== Functions and macros to execute the code unsafe but optimize =====

// --- Macro to get the registers
#define COMMAND data->table_array[0]->content[data->exec_p]
#define OP_CODE (COMMAND >> COMMAND_SHIFT) & COMMAND_MASK
#define R_A data->registers[((COMMAND >> A_SHIFT) & ARG_MASK)]
#define R_B data->registers[((COMMAND >> B_SHIFT) & ARG_MASK)]
#define R_C data->registers[((COMMAND >> C_SHIFT) & ARG_MASK)]

// --- Inline for a conditional move
#define DO_COND_MOVE \
    if(R_C != 0) R_A = R_B;

// --- Inline for a array index
#define DO_ARRAY_INDEX \
    R_A = data->table_array[(unsigned int) R_B]->content[(unsigned int) R_C];

// --- Inline for an array update
#define DO_ARRAY_UPDATE \
    data->table_array[(unsigned int) R_A]->content[(unsigned int) R_B] = R_C;

// --- Inline for an addition
#define DO_ADD \
    R_A = R_B + R_C;

// --- Inline for a multiplication
#define DO_MULT \
    R_A = R_B * R_C;

// --- Inline for a division
#define DO_DIV \
    R_A = (unsigned int) R_B / (unsigned int) R_C;

// --- Inline for a nand
#define DO_NAND \
    R_A = ~(R_B & R_C);

// --- Inline for a halt
#define DO_HALT \
    return;

// --- Inline for a allocation
#define DO_ALLOC \
    R_B = allocate_table(data, (unsigned int) R_C);

// --- Inline for a free
#define DO_FREE \
    free_table(data, (unsigned int) R_C);

// --- Inline for an output
#define DO_OUTPUT \
    printf("%c", R_C);

// --- Inline for a char reader
#define DO_INPUT \
    R_C = (int) CHAR_READER(); \
    if((char) R_C == '\n') R_C = -1;

// --- Inline for a program loading
#define DO_LOAD_PROG \
    save = R_C; \
    if((unsigned int) R_B != 0) { \
        data->table_array[0] = realloc(data->table_array[0], (data->table_array[(unsigned int) R_B]->size + 1) * sizeof(int)); \
        data->table_array[0]->size = data->table_array[(unsigned int) R_B]->size; \
        memcpy((void *) data->table_array[0]->content, (void *) data->table_array[(unsigned int) R_B]->content, data->table_array[0]->size * sizeof(int)); \
    } \
    data->exec_p = (unsigned int) save;

// --- Inline for an ortho
#define DO_ORTHO \
    data->registers[(int) ((COMMAND >> A_SPEC_SHIFT) & ARG_MASK)] = (int) (COMMAND & DATA_MASK);

// --- Inline for jumping to the next instruction
#define JUMP_NEXT \
    data->exec_p++; \
    goto *labels[OP_CODE];

// --- Inline for jumping to the current instruction
#define JUMP_CURRENT \
    goto *labels[OP_CODE];

// --- Execute a command by dispatching it
void execute(machine_data_t *data) {

    // Declare the useful variables
    int save;

    // Declare the label array
    void *labels[] = {
        &&COND_MOVE,
        &&ARRAY_INDEX,
        &&ARRAY_UPDATE,
        &&ADD,
        &&MULT,
        &&DIV,
        &&NAND,
        &&HALT,
        &&ALLOC,
        &&FREE,
        &&OUTPUT,
        &&INPUT,
        &&LOAD_PROG,
        &&ORTHO
    };

    // Start the first command
    goto *labels[OP_CODE];

    // --- Labels for threaded execution

    COND_MOVE: // Do a conditional move
        DO_COND_MOVE
        JUMP_NEXT

    ARRAY_INDEX: // Do an array access
        DO_ARRAY_INDEX
        JUMP_NEXT

    ARRAY_UPDATE: // Do an array update
        DO_ARRAY_UPDATE
        JUMP_NEXT

    ADD: // Do an addition
        DO_ADD
        JUMP_NEXT

    MULT: // Do a multiplication
        DO_MULT
        JUMP_NEXT

    DIV: // Do a division
        DO_DIV
        JUMP_NEXT

    NAND: // Do a not-and
        DO_NAND
        JUMP_NEXT

    HALT: // Do an halt
        DO_HALT

    ALLOC: // Do a table allocation
        DO_ALLOC
        JUMP_NEXT

    FREE: // Free a table
        DO_FREE
        JUMP_NEXT

    OUTPUT: // Output a char in the console
        DO_OUTPUT
        JUMP_NEXT

    INPUT: // Input a char in the console
        DO_INPUT
        JUMP_NEXT

    LOAD_PROG: // Load a program
        DO_LOAD_PROG
        JUMP_CURRENT

    ORTHO: // Load a value
        DO_ORTHO
        JUMP_NEXT

}
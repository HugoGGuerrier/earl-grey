#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug_executer.h"
#include "machine.h"
#include "utils.h"


// ===== Functions to execute a bytecode safely =====

// --- Internal function declarations
static void _do_cond_move(machine_data_t *data, int a, int b, int c);
static void _do_array_index(machine_data_t *data, int a, int b, int c);
static void _do_array_update(machine_data_t *data, int a, int b, int c);
static void _do_add(machine_data_t *data, int a, int b, int c);
static void _do_multiplication(machine_data_t *data, int a, int b, int c);
static void _do_division(machine_data_t *data, int a, int b, int c);
static void _do_nand(machine_data_t *data, int a, int b, int c);
static void _do_halt(machine_data_t *data);
static void _do_allocation(machine_data_t *data, int b, int c);
static void _do_free(machine_data_t *data, int c);
static void _do_output(machine_data_t *data, int c);
static void _do_input(machine_data_t *data, int c);
static void _do_load_prog(machine_data_t *data, int b, int c);
static void _do_ortho(machine_data_t *data, unsigned int command);

// --- Do a conditional move
static void _do_cond_move(machine_data_t *data, int a, int b, int c) {
    if(data->registers[c] != 0) {
        data->registers[a] = data->registers[b];
    }
}

// --- Do an array index fetching
static void _do_array_index(machine_data_t *data, int a, int b, int c) {
    unsigned int r_b = data->registers[b];
    unsigned int r_c = data->registers[c];

    // Verify the table index
    if(r_b < data->table_array_size) {
        // Verify the plate index
        if(r_c < data->table_array[r_b]->size) {
            data->registers[a] = data->table_array[r_b]->content[r_c];
        } else {
            raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to access a plate out of bounds");
        }
    } else {
        raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to access a table out of bounds");
    }
}

// --- Do an array update
static void _do_array_update(machine_data_t *data, int a, int b, int c) {
    unsigned int r_a = data->registers[a];
    unsigned int r_b = data->registers[b];
    int r_c = data->registers[c];

    // Verify the table index
    if(r_a < data->table_array_size) {
        // Verify the plate index
        if(r_b < data->table_array[r_a]->size) {
            data->table_array[r_a]->content[r_b] = r_c;
        } else {
            raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to access a plate out of bounds");
        }
    } else {
        raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to access a table out of bounds");
    }
}

// --- Do an addition
static void _do_add(machine_data_t *data, int a, int b, int c) {
    int r_b = data->registers[b];
    int r_c = data->registers[c];

    data->registers[a] = r_b + r_c;
}

// --- Do a multiplication
static void _do_multiplication(machine_data_t *data, int a, int b, int c) {
    int r_b = data->registers[b];
    int r_c = data->registers[c];

    data->registers[a] = r_b * r_c;
}

// --- Do a division (each operand is treated as an unsigned int)
static void _do_division(machine_data_t *data, int a, int b, int c) {
    unsigned int r_b = data->registers[b];
    unsigned int r_c = data->registers[c];

    if(r_c != 0) {
        data->registers[a] = r_b / r_c;
    } else {
        raise_machine_error(data, DIVIDE_BY_ZERO, "Tried to divide by 0, oh shi-");
    }
}

// --- Do a NAND
static void _do_nand(machine_data_t *data, int a, int b, int c) {
    int r_b = data->registers[b];
    int r_c = data->registers[c];

    data->registers[a] = ~(r_b & r_c);
}

// --- Stop the virtual machine
static void _do_halt(machine_data_t *data) {
    data->flags &= ~RUNNING_FLAG;
}

// --- Do an allocation
static void _do_allocation(machine_data_t *data, int b, int c) {
    unsigned int r_c = data->registers[c];

    unsigned int new_index = allocate_table(data, r_c);
    memset(data->table_array[new_index]->content, 0, data->table_array[new_index]->size * sizeof(int));

    data->registers[b] = new_index;
}

// --- Do a free
static void _do_free(machine_data_t *data, int c) {
    unsigned int r_c = data->registers[c];

    if(r_c < data->table_array_size && r_c > 0) {
        if(data->table_array[r_c] != NULL) {
            free_table(data, r_c);
        } else {
            raise_machine_error(data, BAD_FREE_POINTER, "Tried to free a NULL table");
        }
    } else {
        raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to free a table out of bounds");
    }
}

// --- Do an output
static void _do_output(machine_data_t *data, int c) {
    int r_c = data->registers[c];

    if(r_c >= 0 && r_c <= 255) {
        printf("%c", r_c);
    } else {
        raise_machine_error(data, OUTPUT_ERROR, "Tried to output a value not between 0 and 255");
    }
}

// --- Do an input
static void _do_input(machine_data_t *data, int c) {
    char read = CHAR_READER();

    if(read == '\n') {
        data->registers[c] = -1;
    } else {
        data->registers[c] = (int) read;
    }
}

// --- Do a program load
static void _do_load_prog(machine_data_t *data, int b, int c) {
    unsigned int r_b = data->registers[b];
    unsigned int r_c = data->registers[c];

    // Check the table index
    if(r_b < data->table_array_size) {

        table_t *new_table;

        // If the table index is 0, no need to copy the table
        if(r_b != 0) {
            new_table = (table_t *) malloc((data->table_array[r_b]->size + 1) * sizeof(int));
            new_table->size = data->table_array[r_b]->size;
            memcpy((void *) new_table->content, (void *) data->table_array[r_b]->content, new_table->size * sizeof(int));

            free(data->table_array[0]);
        } else {
            new_table = data->table_array[0];
        }

        // Set the new command table
        data->table_array[0] = new_table;

        // Check the new execution index
        if(r_c < data->table_array[0]->size) {
            data->exec_p = r_c;
            data->flags |= SKIP_SHIFT_FLAG;
        } else {
            raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to place the execution pointer out of bounds");
        }

    } else {
        raise_machine_error(data, INDEX_OUT_OF_BOUNDS, "Tried to access a table out of bounds");
    }
}

// --- Do an ortho
static void _do_ortho(machine_data_t *data, unsigned int command) {
    int a = get_special_a(command);
    int value = get_special_value(command);

    data->registers[a] = value;
}

// --- Execute a command by dispatching it
void debug_execute(machine_data_t *data) {

    // Open the output file for the debug trace
    FILE *exec_file = NULL;
    if(data->flags & LOG_FLAG) {
        exec_file = fopen(data->log_file, "w");
    }

    // Declare and get the three args and the command
    int a, b, c, command;

    // While there are more commands and not error
    while(data->flags & RUNNING_FLAG && data->exec_p < data->table_array[0]->size && data->error->error_code == 0) {

        // Get the current command
        command = data->table_array[0]->content[data->exec_p];

        // Write the command and registers
        if(exec_file != NULL) {
            write_step(data, command, exec_file);
        }

        // Get the three arguments
        a = get_arg_a(command);
        b = get_arg_b(command);
        c = get_arg_c(command);

        // Parse the command
        switch(get_command(command)) {

        case 0: // Conditional move
            _do_cond_move(data, a, b, c);
            break;

        case 1: // Array index
            _do_array_index(data, a, b, c);
            break;

        case 2: // Array update
            _do_array_update(data, a, b, c);
            break;

        case 3: // Add
            _do_add(data, a, b, c);
            break;

        case 4: // Multiply
            _do_multiplication(data, a, b, c);
            break;

        case 5: // Divide
            _do_division(data, a, b, c);
            break;

        case 6: // Not and
            _do_nand(data, a, b, c);
            break;

        case 7: // Halt !
            _do_halt(data);
            break;

        case 8: // Allocate
            _do_allocation(data, b, c);
            break;

        case 9: // Free
            _do_free(data, c);
            break;

        case 10: // Output
            _do_output(data, c);
            break;

        case 11: // Input
            _do_input(data, c);
            break;

        case 12: // Load prog
            _do_load_prog(data, b, c);
            break;

        case 13: // Ortho
            _do_ortho(data, command);
            break;

        default:
            raise_machine_error(data, COMMAND_ERROR, "Unknown command");
            break;

        }

        // Increase the execution pointer
        if(data->flags & SKIP_SHIFT_FLAG) {
            data->flags &= ~SKIP_SHIFT_FLAG;
        } else {
            data->exec_p++;
        }

    }

    // Close the execution file
    if(exec_file != NULL) {
        fclose(exec_file);
    }

}
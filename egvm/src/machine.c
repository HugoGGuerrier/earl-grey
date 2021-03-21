#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "machine.h"
#include "utils.h"
#include "executer.h"
#include "debug_executer.h"


// ===== Functions to manipulate the machine =====

// --- Function declarations
static void _double_table_array(machine_data_t *data);
static void _clean_up(machine_data_t *data);

// --- Double the table collection size
static void _double_table_array(machine_data_t *data) {

    data->table_array_cap *= 2;
    data->table_array = (table_t **) realloc((void *) data->table_array, data->table_array_cap * sizeof(table_t *));

}

// --- Clean up the memory
static void _clean_up(machine_data_t *data) {

    // Set all freelist to null
    table_t **freel = data->free_start;
    while (freel != NULL) {
        table_t **next = (table_t **) *freel;
        *freel = NULL;
        freel = next;
    }
    data->free_start = NULL;

    // Clean the table array
    for(unsigned int i = 0 ; i < data->table_array_size ; i++) {
        if(data->table_array[i] != NULL) {
            free(data->table_array[i]);
        }
    }
    free(data->table_array);

}

// --- Function to allocate a new plate table and return its index
unsigned int allocate_table(machine_data_t *data, unsigned int size) {

    // Prepare the new index
    unsigned int new_table_index;

    // Check if there is a free space in the array
    if(data->free_start != NULL) {

        new_table_index = (unsigned int) (data->free_start - data->table_array);
        data->free_start = (table_t **) *data->free_start;

    } else {

        new_table_index = data->table_array_size;

        // Check the array size and double it if needed
        if(data->table_array_size >= data->table_array_cap) {
            _double_table_array(data);
        }

        data->table_array_size++;

    }

    // Create a new table and increase the size
    data->table_array[new_table_index] = (table_t *) calloc(size + 1, sizeof(int));
    data->table_array[new_table_index]->size = size;

    return new_table_index;

}

// --- Function to free a plate table
void free_table(machine_data_t *data, unsigned int index) {

    free(data->table_array[index]);
    data->table_array[index] = NULL;

    if(index == data->table_array_size - 1) {

        // Handle the size changments
        data->table_array_size--;

    } else {
        
        // Update the free list
        data->table_array[index] = (table_t *) data->free_start;
        data->free_start = &data->table_array[index];

    }

}

// --- Function to raise an execution error
void raise_machine_error(machine_data_t *data, int error_code, char *error_message) {
    data->error->error_code = error_code;
    data->error->error_offset = data->exec_p;
    data->error->error_message = error_message;
}

// --- Main function of the machine
void run_machine(machine_data_t *data) {

    // Initialize the machine data for the execution
    data->exec_p = 0;
    for(int i = 0 ; i < REGISTER_NUMBER ; i++) {
        data->registers[i] = 0;
    }
    data->table_array_size = 1;
    data->table_array_cap = 1;
    data->free_start = NULL;

    // Read the binary file, get its code and initialise the code pointer
    data->table_array = (table_t **) malloc(sizeof(table_t *));
    data->table_array[0] = read_egb_file(data->egb_file_name);

    // Execute the code in the wanted mode
    if(data->flags & DEBUG_FLAG) {
        debug_execute(data);
    } else {
        execute(data);
    }

    // Clean up the table array
    _clean_up(data);

}

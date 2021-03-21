#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"
#include "machine.h"

// OS specific imports
#ifdef EG_UNIX
    #include <termios.h>
#elif EG_WINDOWS
    // Do Windows imports
#elif EG_MAC
    // Do MacOS imports
#endif


// ===== Untils functions =====

// --- File variables
static const char *_command_names[14] = {"MOVE", "ARIN", "ARUP", "ADDI", "MULT", "DIVI", "NAND", "HALT", "ALOC", "FREE", "OUTP", "INPT", "LOAD", "ORTH"};

// --- Read the egb file and return a new table with its content
table_t *read_egb_file(const char *file_name) {

    // Open the file and get its size in int
    FILE *file = fopen(file_name, "r");
    fseek(file, 0L, SEEK_END);
    long file_size = ftell(file) / 4;
    rewind(file);

    // Allocate the memory for the result
    table_t *res = (table_t *) malloc((file_size + 1) * sizeof(int));
    res->size = file_size;
    int *buffer_p = res->content;

    // Read the file and store it into the buffer
    for(int i = 0 ; i < file_size ; i++) {
        if(fread(buffer_p, 4, 1, file) == 1) {
            // TODO : Test the endianess of the computer
            *buffer_p = REVERSER(*buffer_p);

            buffer_p++;
        }
    }
    
    printf("%d\n", res->content[0]);

    // Close the file
    fclose(file);

    // Return the result
    return res;

}

// --- Write the execution trace step in a file
void write_step(machine_data_t *data, unsigned int command, FILE *file) {

    // Write the current command and its args
    int a, b, c, s_a, v, com;
    com = (command >> 28) & 0xF;
    a = get_arg_a(command);
    b = get_arg_b(command);
    c = get_arg_c(command);
    s_a = get_special_a(command);
    v = get_special_value(command);

    if(com <= 13) {
        if(com != 13) {
            fprintf(file, "%s : %d %d %d", _command_names[com], a, b, c);
        } else {
            fprintf(file, "%s : %d %d", _command_names[com], s_a, v);
        }
    }

    // Write the current registers state
    fprintf(file, "\t\t[");
    for(int i = 0 ; i < REGISTER_NUMBER ; i++) {
        fprintf(file, "%d ", data->registers[i]);
    }
    fprintf(file, "]\n");

}

// --- Change the file extension
char *change_extension(char *file_name, char *new_extension) {

    // Get the last dot position
    unsigned int ld_pos = 0;
    unsigned int i = 0;
    char cur_char = file_name[0];
    while(cur_char != '\0') {
        cur_char = file_name[i];
        if(cur_char == '.') {
            ld_pos = i;
        }
        i++;
    }

    // Get the extension size
    unsigned int extension_size = strlen(new_extension);

    // Prepare the memory to store the result and copy the memory
    char *res = (char *) malloc((ld_pos + extension_size) * sizeof(char));
    memcpy(res, file_name, ld_pos * sizeof(char));
    strcat(res, ".");
    strcat(res, new_extension);

    // Return the result
    return res;
    
}

// --- Reverse an int endianess
int reverse(int to_reverse) {
    return ((to_reverse>>24)&0xFF) | ((to_reverse<<8)&0xFF0000) | ((to_reverse>>8)&0xFF00) | ((to_reverse<<24)&0xFF000000);
}

// --- The identity function
int identity(int ident) {
    return ident;
}

// --- UNIX | Read a char from the console
char unix_read_char() {

    // Save the old terminal config
    static struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);

    // Set the buffering to off
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read the char
    char res = getchar();

    // Reset the terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return res;

}

// --- Windows | Read a char from the console

// --- MacOS | Read a char from the console

// --- Get the command number
int get_command(unsigned int command) {
    return (int) ((command >> COMMAND_SHIFT) & COMMAND_MASK);
}

// --- Get the A argument
int get_arg_a(unsigned int command) {
    return (int) ((command >> A_SHIFT) & ARG_MASK);
}

// --- Get the B argument
int get_arg_b(unsigned int command) {
    return (int) ((command >> B_SHIFT) & ARG_MASK);
}

// --- Get the C argument
int get_arg_c(unsigned int command) {
    return (int) ((command >> C_SHIFT) & ARG_MASK);
}

// --- Get the special A argument
int get_special_a(unsigned int command) {
    return (int) ((command >> A_SPEC_SHIFT) & ARG_MASK);
}

// --- Get the special value
int get_special_value(unsigned int command) {
    return (int) (command & DATA_MASK);
}

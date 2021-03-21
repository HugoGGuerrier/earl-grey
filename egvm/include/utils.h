#ifndef READER_H
#define READER_H

#include "machine.h"

// OS specific defines
#ifdef EG_UNIX
    #include <byteswap.h>

    #define REVERSER __bswap_32
    #define CHAR_READER unix_read_char
#elif EG_WINDOWS
    // Do Windows define
#elif EG_MAC
    // Do MacOS define
#endif

#define COMMAND_MASK 0xF
#define ARG_MASK 7
#define DATA_MASK 0x1FFFFFF

#define COMMAND_SHIFT 28
#define A_SHIFT 6
#define B_SHIFT 3
#define C_SHIFT 0
#define A_SPEC_SHIFT 25


// ===== Exported functions =====

table_t *read_egb_file(const char *file_name);
void write_step(machine_data_t *data, unsigned int command, FILE *file);
char *change_extension(char *file_name, char *new_extension);

int reverse(int to_reverse);
int identity(int ident);

char unix_read_char();

int get_command(unsigned int command);
int get_arg_a(unsigned int command);
int get_arg_b(unsigned int command);
int get_arg_c(unsigned int command);

int get_special_a(unsigned int command);
int get_special_value(unsigned int command);


#endif
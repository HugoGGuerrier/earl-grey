#ifndef MAIN_H
#define MAIN_H

#define AST_MASK 0b1
#define VERBOSE_MASK 0b10

#include <stdio.h>


// ===== Structure definitions =====

// --- This structure contains all configuration of the compiler
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


// ===== Exported function definitions =====

void raise_error(compiler_error_t *error, int code, const char *message);
int main(int argc, char *argv[]);


#endif
#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "main.h"


// ===== Structure definitions =====

// --- Structure for containing all configuration of the compiler
typedef struct {
    unsigned int flags;
    char *input_file_name;
    FILE *input_file;
    char *output_file_name;
    FILE *output_file;
    char **include_dirs;
} compiler_settings_t;

// --- Structure for handling compiler errors
typedef struct {
    int error_code;
    const char *error_message;
} compiler_error_t;

// --- Structure for containing data for the compiler
typedef struct {
    compiler_settings_t *settings;
    compiler_error_t *error;
} compiler_data_t;


// ===== Exported function definitions =====

void compile(AST_Prog prog, compiler_data_t *data);
void raise_error(compiler_error_t *error, int code, const char *message);

#endif
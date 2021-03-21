#ifndef COMPILER_H
#define COMPILER_H

#include "ast.h"
#include "main.h"


// ===== Structure definitions =====

// --- Contains all data about the compilation
typedef struct {

} compiler_data_t;


// ===== Exported function definitions =====

void compile(AST_Prog prog, compiler_settings_t *settings, compiler_error_t *error);


#endif
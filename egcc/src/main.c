#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "main.h"
#include "utils.h"
#include "ast.h"
#include "ast_printer.h"
#include "compiler.h"
#include "parser.tab.h"


// ===== Main functions =====

// --- Function to parse the arguments
static int _parse_args(int argc, char *argv[], compiler_settings_t *settings) {

    // Verify the arguments number, else display the help
    if (argc < 2) {
        printf("Usage : egcc [OPTIONS] <FILE.eg>\n");
        printf("Use -h option to display help\n");
        return 1;
    }

    // Read the argument to fill the settings
    for(int i = 1 ; i < argc ; i++) {
        char *current_arg = argv[i];
        if(current_arg[0] == '-') {

             // Get the verbose flag
            if(strcmp("-h", current_arg) == 0) {
                settings->flags |= HELP_MASK;
            }

            // Get the include dirs
            if(strcmp("-i", current_arg) == 0) {
                i++;
                settings->include_dirs = str_split(argv[i], ':');
            } else

            // Get the output file
            if(strcmp("-o", current_arg) == 0) {
                i++;
                settings->output_file_name = argv[i];
            } else

            // Get the verbose flag
            if(strcmp("-v", current_arg) == 0) {
                settings->flags |= VERBOSE_MASK;
            }
            
            // Get the AST flag
            if(strcmp("--ast", current_arg) == 0) {
                settings->flags |= AST_MASK;
            }

        } else {

            // Get the input file
            settings->input_file_name = current_arg;

        }
    }

    return 0;
}

// --- Display the help
static void _display_help() {
    printf("egcc : The earl grey compiler\n");
    printf("Version : %s\n\n", EGCC_VERSION);
    printf("Usage : egcc [OPTIONS] <FILE.eg>\n\n");
    printf("Options :\n");
    printf("    -h : Display this help menu\n");
    printf("    -i <dir1:dir2> : Precise the include directories\n");
    printf("    -o <OUTPUT.egb> : Set the output file\n");
    printf("    -v : Enable the verbose mode\n");
    printf("\n");
    printf("    --ast : Display the ast before the compilation\n");
}

// --- The main function
int main(int argc, char *argv[]) {

    // Prepare the settings and the error structures
    compiler_settings_t settings;
    settings.flags = 0;
    settings.input_file_name = NULL;
    settings.input_file = NULL;
    settings.output_file_name = NULL;
    settings.output_file = NULL;
    settings.include_dirs = NULL;

    compiler_error_t error;
    error.error_code = 0;
    error.error_message = NULL;

    // Do the argument parsing
    if(_parse_args(argc, argv, &settings)) {
        return 1;
    }

    // Display the help of needed
    if(settings.flags & HELP_MASK) {
        _display_help();
        return 0;
    }

    // Verify that the input file exists
    if(access(settings.input_file_name, F_OK)) {
        printf("\"%s\" : File not found\n", settings.input_file_name);
        return 1;
    }

    // Create the output file and open it
    if(settings.output_file_name == NULL) {
        settings.output_file_name = change_extension(settings.input_file_name, "egb");
    }
    settings.output_file = fopen(settings.output_file_name, "w");

    // Open the input file and do the parsing
    settings.input_file = freopen(settings.input_file_name, "r", stdin);
    AST_Prog *prog = (AST_Prog *) malloc(sizeof(AST_Prog));
    if(yyparse(prog)) {
        return 1;
    }

    // If the --ast flag is on, display the AST
    if(settings.flags & AST_MASK) {
        printf("=== AST : \n\n");
        print_ast(*prog);
        printf("\n");
    }

    // Create the data struct
    compiler_data_t data;
    data.settings = &settings;
    data.error = &error;
    data.offset = 0;

    // Do the compilation
    compile(*prog, &data);

    // Clean the memory
    clean_ast(*prog);

    // Close the input and output files
    fclose(settings.output_file);
    fclose(settings.input_file);

    // Verify the error code
    if(error.error_code != 0) {
        fprintf(stderr, "Compilation error (%d) : %s", error.error_code, error.error_message);
        return 1;
    }

    return 0;
}

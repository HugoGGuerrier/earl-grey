#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "main.h"
#include "utils.h"
#include "machine.h"


// ===== Main functions =====

// --- Parse the arguments
int _parse_args(int argc, char *argv[], machine_data_t *data) {

    // Verify the arguments number, else display the help
    if (argc < 2) {
        printf("Usage : egvm [OPTIONS] <FILE.egb>\n\n");
        printf("Options :\n");
        printf("    -d : Enable the debug mode (Execute the bytecode safely)\n");
        printf("    -l : Enable the logging mode !!! Works only in debug mode !!! (Save all instructions read in a file)\n");
        return 1;
    }

    // Read the argument to fill the data
    for(int i = 1 ; i < argc ; i++) {
        char *current_arg = argv[i];
        if(current_arg[0] == '-') {

            // Get the debug flag
            if(strcmp("-d", current_arg) == 0) {
                data->flags |= DEBUG_FLAG;
            } else

            // Get the log flag
            if(strcmp("-l", current_arg) == 0) {
                data->flags |= LOG_FLAG;
            }

        } else {

            // Get the egb file name
            data->egb_file_name = current_arg;

        }
    }

    if(data->flags & LOG_FLAG) {
        data->log_file = change_extension(data->egb_file_name, ".exec");
    }

    return 0;

}

// --- The main function to start the interpretation
int main(int argc, char *argv[]) {

    // Prepare the machine error handler
    machine_error_t error;
    error.error_code = 0;
    error.error_offset = 0;

    // Prepare the machine settings
    machine_data_t data;
    data.error = &error;
    data.flags = 0;
    data.flags |= RUNNING_FLAG;
    data.flags &= ~SKIP_SHIFT_FLAG;

    // Parse the arguments
    if(_parse_args(argc, argv, &data)) {
        return 1;
    }

    // Check if the bytecode file exists
    if(access(data.egb_file_name, F_OK)) {
        printf("\"%s\" : File not found\n", data.egb_file_name);
        return 1;
    }
    
    // Start the machine
    run_machine(&data);

    // Handle the universal machine errors
    if(error.error_code) {
        fprintf(stderr, "Universal machine error (offset %u) : %s\n", error.error_offset, error.error_message);
        return error.error_code;
    }

    // Return the normal exit status
    return 0;
    
}

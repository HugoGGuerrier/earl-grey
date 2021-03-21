#ifndef MACHINE_H
#define MACHINE_H

// Define macros to factorize the OS detection
#if defined(__unix) || defined(unix) || defined (__unix__)
    #define EG_UNIX
#elif defined(_WIN32) || defined(_WIN64)
    #define EG_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
    #define EG_MAC
#endif

// Define the register number
#define REGISTER_NUMBER 8

// Define error code
#define INDEX_OUT_OF_BOUNDS 1
#define BAD_FREE_POINTER 2
#define COMMAND_ERROR 3
#define OUTPUT_ERROR 4
#define DIVIDE_BY_ZERO 5

// Define flags mask
#define RUNNING_FLAG 0b1
#define SKIP_SHIFT_FLAG 0b10
#define LOG_FLAG 0b100
#define DEBUG_FLAG 0b1000


// ===== Structure definitions =====

// This structure contains an error from the machine
typedef struct {
    int error_code;
    unsigned int error_offset;
    char *error_message;
} machine_error_t;

// This structure represent a table
typedef struct {
    unsigned int size;
    int content[];
} table_t;

// This structure contains all information for the machine to run
typedef struct {
    machine_error_t *error;
    char *egb_file_name;
    char *log_file;

    unsigned char flags;

    unsigned int exec_p;
    int registers[REGISTER_NUMBER];

    unsigned int table_array_cap;
    unsigned int table_array_size;
    table_t **free_start;
    table_t **table_array;
} machine_data_t;

// ===== Exported functions =====

void raise_machine_error(machine_data_t *data, int error_code, char *error_message);
void run_machine(machine_data_t *data);
unsigned int allocate_table(machine_data_t *data, unsigned int size);
void free_table(machine_data_t *data, unsigned int index);


#endif
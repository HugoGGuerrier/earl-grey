#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"


// ===== Utils functions =====

// --- Split a string with a delimiter and return a NULL terminated array
char **str_split(char *str, char delimiter) {

    // Prepare the char * array finished by NULL
    char **res = (char **) malloc(sizeof(char *));
    int res_size = 1;
    res[0] = NULL;

    // Iterate over the string and for each delimiter store the previous in the result
    int i = 0;
    int prev = 0;
    char cur_char = str[0];

    while(cur_char != '\0') {
        cur_char = str[i];

        if(cur_char == delimiter || cur_char == '\0') {
            char *tmp = (char *) malloc((i - prev) * sizeof(char));
            memcpy(tmp, (str + prev), (i - prev) * sizeof(char));

            res[res_size - 1] = tmp;
            res_size++;
            res = (char **) realloc((void *)res, res_size * sizeof(char *));

            prev = i + 1;
        }

        i++;
    }

    // Return the result
    return res;

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
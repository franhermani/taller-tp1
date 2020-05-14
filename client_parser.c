#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "client_parser.h"

// Auxiliary function to split params and firm by "(" and ")"
static void split_params_and_firm(char *line, char *params, char *firm);

// Auxiliary function to split params by spaces
static void split_params(char *params, char **params_array);


void parse_line(char *line, char **params_array) {
    int max_length = strlen(line);
    char *params = malloc(max_length), *firm = malloc(max_length);

    split_params_and_firm(line, params, firm);

    max_length = strlen(params);

    char *destiny = malloc(max_length), *path = malloc(max_length),
    *interface = malloc(max_length), *method = malloc(max_length);

    params_array[0] = destiny;
    params_array[1] = path;
    params_array[2] = interface;
    params_array[3] = method;
    params_array[4] = firm;

    split_params(params, params_array);
    free(params);
}

static void split_params_and_firm(char *line, char *params, char *firm) {
    const char delim = '(', end = ')';
    bool is_firm = false;
    int i, j = 0;
    for (i = 0; line[i] != '\0'; i ++) {
        if (line[i] == delim) {
            params[j] = '\0';
            is_firm = true;
            j = 0;
            continue;
        } else if (line[i] == end) {
            firm[j] = line[i];
            firm[j+1] = '\0';
            continue;
        }
        if (is_firm) {
            firm[j] = line[i];
        } else {
            params[j] = line[i];
        }
        j ++;
    }
}

static void split_params(char *params, char **params_array) {
    const char delim = ' ';
    int i, j = 0, k = 0;
    for (i = 0; params[i] != '\0'; i ++) {
        if (params[i] == delim) {
            params_array[j][k] = '\0';
            j ++;
            k = 0;
        } else {
            params_array[j][k] = params[i];
            k ++;
        }
    }
    params_array[j][k] = '\0';
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include <unistd.h>
#include <stdbool.h>

bool has_connection_close(const char *buf) {
    return strstr(buf, "\r\nConnection: close\r\n") != NULL;
}

char *helper_readline(void) {
    size_t cap = 0;
    char *line = NULL;
	ssize_t len = getline(&line, &cap, stdin);
    if (len <= 0) {
        free(line);
        return NULL;
    }
    // strip newline
    if (line[len-1] == '\n') line[len-1] = '\0';
    return line;
}

char **helper_split(const char *str) {
    // TODO: implement splitting by whitespace
    // stub:
    char **tokens = malloc(sizeof(char*) * 2);
    tokens[0] = strdup(str);
    tokens[1] = NULL;
    return tokens;
}

void helper_free_tokens(char **tokens) {
    for (size_t i = 0; tokens[i] != NULL; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include <unistd.h>

int get_status(char *resp) {
	int status = 0;
	char *p = strstr(resp, "HTTP/1.1 ");
    if (p) status = atoi(p + 9);
	return status;
}

char *extract_cookie(char *resp) {
    char *cookie = NULL;
    char *start = strstr(resp, "Set-Cookie: ");
    if (start != NULL) {
        start += strlen("Set-Cookie: ");
        char *end = strstr(start, "\r\n");
        if (end != NULL) {
            // If there’s a semicolon before the CRLF, cut there
            char *semi = memchr(start, ';', end - start);
            if (semi != NULL) {
                end = semi;
            }
            size_t len = end - start;
            cookie = malloc(len + 1);
            if (cookie) {
                memcpy(cookie, start, len);
                cookie[len] = '\0';
            }
        }
    }
    return cookie;
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

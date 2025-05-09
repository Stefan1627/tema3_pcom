#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "parson.h"
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include "parson.h"

char *strip_headers(const char *resp) {
    const char *sep = strstr(resp, "\r\n\r\n");
    if (!sep) {
        fprintf(stderr, "No header/body separator found in HTTP response\n");
        return NULL;
    }
    sep += 4;  /* skip past the "\r\n\r\n" */
    return strdup(sep);
}

char *extract_token(const char *resp) {
    if (!resp) return NULL;

    JSON_Value *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "Error: failed to parse JSON.\n");
        return NULL;
    }

    JSON_Object *root_obj = json_value_get_object(root_value);
    if (!root_obj) {
        fprintf(stderr, "Error: JSON root is not an object.\n");
        json_value_free(root_value);
        return NULL;
    }

    const char *token = json_object_get_string(root_obj, "token");
    if (!token) {
        fprintf(stderr, "Error: no \"token\" field in JSON.\n");
        json_value_free(root_value);
        return NULL;
    }

    // Duplicate the token so it survives after we free the JSON_Value
    char *token_copy = malloc(strlen(token) + 1);
    if (token_copy) {
        strcpy(token_copy, token);
    } else {
        fprintf(stderr, "Error: failed to allocate memory for token.\n");
    }

    json_value_free(root_value);
    return token_copy;
}

void print_users(const char *resp) {
    /* Parse the JSON text */
    JSON_Value *root_val = json_parse_string(resp);
    if (!root_val) {
        fprintf(stderr, "Invalid JSON\n");
        return;
    }

    JSON_Object *root_obj = json_value_get_object(root_val);
    if (!root_obj) {
        fprintf(stderr, "Expected JSON object at root\n");
        json_value_free(root_val);
        return;
    }

    /* Get the "users" array */
    JSON_Array *users = json_object_get_array(root_obj, "users");
    if (!users) {
        fprintf(stderr, "No \"users\" array found\n");
        json_value_free(root_val);
        return;
    }

    size_t count = json_array_get_count(users);
    for (size_t i = 0; i < count; i++) {
        JSON_Object *user = json_array_get_object(users, i);
        if (!user) continue;

        const char *username = json_object_get_string(user, "username");
        const char *password = json_object_get_string(user, "password");
        if (!username) username = "(null)";
        if (!password) password = "(null)";

        /* Print as “#1 test:test” etc. */
        printf("#%zu %s:%s\n", i + 1, username, password);
    }

    json_value_free(root_val);
}


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

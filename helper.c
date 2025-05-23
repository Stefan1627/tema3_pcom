// 324CC Stefan CALMAC
#include "helper.h"
#include "parson.h"

/**
 * Remove HTTP headers from the response and return a newly allocated string
 * containing only the body.
 *
 * @param resp  Full HTTP response (headers + "\r\n\r\n" + body)
 * @return      Malloc’d copy of the body on success, or NULL if no separator found
 */
char *strip_headers(const char *resp) {
    const char *sep = strstr(resp, "\r\n\r\n");
    if (!sep) {
        fprintf(stderr, "No header/body separator found in HTTP response\n");
        return NULL;
    }
    sep += 4;  /* skip past the "\r\n\r\n" */
    return strdup(sep);
}

/**
 * Check whether the given string contains any whitespace characters.
 *
 * @param str   Null-terminated string to inspect
 * @return      1 if any isspace() character is found, 0 otherwise
 */
int contains_space(const char *str) {
    while (*str) {
        if (isspace((unsigned char)*str))
            return 1;
        str++;
    }
    return 0;
}

/**
 * Parse an integer "id" from a JSON-style response string.
 *
 * @param resp  String containing JSON with a numeric "id" field
 * @return      The parsed integer value, or -1 if missing/malformed
 */
int extract_id(const char *resp) {
    const char *key = "\"id\"";
    const char *p = strstr(resp, key);
    if (!p) return -1;               // "id" key not found
    p += strlen(key);                // move past "\"id\""
    p = strchr(p, ':');              // find the ':' after "id"
    if (!p) return -1;               // malformed JSON
    p++;                             // move past ':'
    // skip whitespace
    while (*p && isspace((unsigned char)*p)) {
        p++;
    }
    // parse integer
    return (int)strtol(p, NULL, 10);
}

/**
 * Parse a JSON string and extract the "token" field.
 *
 * @param resp  JSON text containing a "token" member
 * @return      Malloc’d copy of the token, or NULL on parse/error
 */
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

    // Duplicate so it outlives the JSON structure
    char *token_copy = malloc(strlen(token) + 1);
    if (token_copy) {
        strcpy(token_copy, token);
    } else {
        fprintf(stderr, "Error: failed to allocate memory for token.\n");
    }

    json_value_free(root_value);
    return token_copy;
}

/**
 * Print details of a movie collection from its JSON representation.
 * Outputs title, owner, and a numbered list of movies (id + title).
 *
 * @param resp  JSON string containing "title", "owner", and array "movies"
 */
void print_collection_details(const char *resp) {
    JSON_Value *root_val = json_parse_string(resp);
    if (!root_val) {
        fprintf(stderr, "Error: failed to parse JSON\n");
        return;
    }

    JSON_Object *root_obj = json_value_get_object(root_val);
    const char *title = json_object_get_string(root_obj, "title");
    const char *owner = json_object_get_string(root_obj, "owner");

    printf("title: %s\n", title ? title : "");
    printf("owner: %s\n", owner ? owner : "");

    JSON_Array *movies = json_object_get_array(root_obj, "movies");
    size_t count = json_array_get_count(movies);
    for (size_t i = 0; i < count; i++) {
        JSON_Object *movie = json_array_get_object(movies, i);
        int id = (int)json_object_get_number(movie, "id");
        const char *m_title = json_object_get_string(movie, "title");
        printf("#%d: %s\n", id, m_title ? m_title : "");
    }

    json_value_free(root_val);
}

/**
 * Print a list of collections (id and title) from a JSON response.
 *
 * @param resp  JSON text containing an array "collections"
 */
void print_collections(const char *resp) {
    JSON_Value *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "ERROR: Invalid JSON\n");
        return;
    }

    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Array *arr = json_object_get_array(root_object, "collections");
    if (!arr) {
        fprintf(stderr, "ERROR: No \"collections\" array found\n");
        json_value_free(root_value);
        return;
    }

    size_t count = json_array_get_count(arr);
    for (size_t i = 0; i < count; ++i) {
        JSON_Object *coll = json_array_get_object(arr, i);
        long id = (long)json_object_get_number(coll, "id");
        const char *title = json_object_get_string(coll, "title");
        if (title) {
            printf("#%ld: %s\n", id, title);
        }
    }

    json_value_free(root_value);
}

/**
 * Print details of a single movie from its JSON representation.
 * Outputs title, year, description, and rating.
 *
 * @param resp  JSON string containing movie fields
 */
void print_movie_details(const char *resp) {
    JSON_Value *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "ERROR: Failed to parse JSON\n");
        return;
    }

    JSON_Object *movie = json_value_get_object(root_value);
    if (!movie) {
        fprintf(stderr, "ERROR: JSON is not an object\n");
        json_value_free(root_value);
        return;
    }

    const char *title       = json_object_get_string(movie, "title");
    double      year_num    = json_object_get_number(movie, "year");
    const char *description = json_object_get_string(movie, "description");
    const char *rating      = json_object_get_string(movie, "rating");

    printf("title: %s\n",       title       ? title       : "(no title)");
    printf("year: %.0f\n",      year_num    );
    printf("description: %s\n", description ? description : "(no description)");
    printf("rating: %s\n",      rating      ? rating      : "(no rating)");

    json_value_free(root_value);
}

/**
 * Print a list of movies (id and title) from a JSON response.
 *
 * @param resp  JSON text containing an array "movies"
 */
void print_movies(const char *resp) {
    JSON_Value *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "ERROR: Failed to parse JSON\n");
        return;
    }

    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Array  *movies      = json_object_get_array(root_object, "movies");
    if (!movies) {
        fprintf(stderr, "ERROR: No \"movies\" array in JSON\n");
        json_value_free(root_value);
        return;
    }

    size_t count = json_array_get_count(movies);
    for (size_t i = 0; i < count; i++) {
        JSON_Object *movie = json_array_get_object(movies, i);
        int id = (int)json_object_get_number(movie, "id");
        const char *title = json_object_get_string(movie, "title");
        printf("#%d %s\n", id, title ? title : "(no title)");
    }

    json_value_free(root_value);
}

/**
 * Print a list of users (username:password) from a JSON response.
 *
 * @param resp  JSON text containing an array "users"
 */
void print_users(const char *resp) {
    JSON_Value *root_val = json_parse_string(resp);
    if (!root_val) {
        fprintf(stderr, "Invalid JSON\n");
        return;
    }

    JSON_Object *root_obj = json_value_get_object(root_val);
    JSON_Array  *users    = json_object_get_array(root_obj, "users");
    if (!users) {
        fprintf(stderr, "No \"users\" array found\n");
        json_value_free(root_val);
        return;
    }

    size_t count = json_array_get_count(users);
    for (size_t i = 0; i < count; i++) {
        JSON_Object *user = json_array_get_object(users, i);
		int id = (int)json_object_get_number(user, "id");
        const char *username = json_object_get_string(user, "username");
        const char *password = json_object_get_string(user, "password");
        printf("#%d %s:%s\n",
               id,
               username ? username : "(null)",
               password ? password : "(null)");
    }

    json_value_free(root_val);
}

/**
 * Extract the HTTP status code from a response string.
 *
 * @param resp  Full HTTP response starting with "HTTP/1.1 XXX ..."
 * @return      The integer status code, or 0 if not found
 */
int get_status(char *resp) {
    int status = 0;
    char *p = strstr(resp, "HTTP/1.1 ");
    if (p) status = atoi(p + 9);
    return status;
}

/**
 * Parse a JSON error message from the response and print it along with
 * the HTTP status code.
 *
 * @param status_code  HTTP status code to display
 * @param resp         Full HTTP response containing a JSON "error" field
 */
void print_http_error(int status_code, const char *resp) {
    const char *body = strchr(resp, '{');
    if (!body) {
        fprintf(stderr, "ERROR: no JSON body found\n");
        return;
    }

    const char *key = strstr(body, "\"error\"");
    if (!key) {
        fprintf(stderr, "ERROR: no \"error\" key in JSON\n");
        return;
    }

    const char *colon = strchr(key, ':');
    if (!colon) {
        fprintf(stderr, "ERROR: malformed JSON (no colon after \"error\")\n");
        return;
    }
    colon++;
    while (*colon == ' ' || *colon == '\t') colon++;
    if (*colon != '"') {
        fprintf(stderr, "ERROR: malformed JSON (\"error\" value not quoted)\n");
        return;
    }

    const char *msg_start = colon + 1;
    const char *msg_end   = strchr(msg_start, '"');
    if (!msg_end) {
        fprintf(stderr, "ERROR: malformed JSON (unterminated string)\n");
        return;
    }
    size_t msg_len = msg_end - msg_start;
    char message[256];
    if (msg_len >= sizeof(message)) msg_len = sizeof(message) - 1;
    memcpy(message, msg_start, msg_len);
    message[msg_len] = '\0';

    printf("ERROR: %d %s\n", status_code, message);
}

/**
 * Extract the first "Set-Cookie" header value from an HTTP response.
 *
 * @param resp  Full HTTP response containing "Set-Cookie: name=value"
 * @return      Malloc’d cookie string (name=value) or NULL if not found
 */
char *extract_cookie(char *resp) {
    char *cookie = NULL;
    char *start = strstr(resp, "Set-Cookie: ");
    if (start != NULL) {
        start += strlen("Set-Cookie: ");
        char *end = strstr(start, "\r\n");
        if (end != NULL) {
            char *semi = memchr(start, ';', end - start);
            if (semi) end = semi;
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

/**
 * Create and return a connected TCP socket to the server defined by
 * HOST and PORT. Exits on failure.
 *
 * @return  Socket file descriptor for the established connection
 */
int setup_conn(void) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(HOST);
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }

    return sockfd;
}

/**
 * Read a line from stdin, strip the trailing newline, and return it.
 *
 * @return  Malloc’d string without the newline, or NULL on EOF/error
 */
char *helper_readline(void) {
    size_t cap = 0;
    char *line = NULL;
    ssize_t len = getline(&line, &cap, stdin);
    if (len <= 0) {
        free(line);
        return NULL;
    }
    if (line[len-1] == '\n') line[len-1] = '\0';
    return line;
}

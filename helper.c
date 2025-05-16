#include "helper.h"
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

int contains_space(const char *str) {
    while (*str) {
        if (isspace((unsigned char)*str))
            return 1;
        str++;
    }
    return 0;
}

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


void print_collection_details(const char *resp) {
    /* Parse the JSON text */
    JSON_Value *root_val = json_parse_string(resp);
    if (!root_val) {
        fprintf(stderr, "Error: failed to parse JSON\n");
        return;
    }

    /* Get the root object */
    JSON_Object *root_obj = json_value_get_object(root_val);

    /* Extract title and owner */
    const char *title = json_object_get_string(root_obj, "title");
    const char *owner = json_object_get_string(root_obj, "owner");

    /* Print header fields */
    printf("title: %s\n", title ? title : "");  
    printf("owner: %s\n", owner ? owner : "");

    /* Extract the movies array */
    JSON_Array *movies = json_object_get_array(root_obj, "movies");
    size_t count = json_array_get_count(movies);

    /* Iterate over each movie */
    for (size_t i = 0; i < count; i++) {
        JSON_Object *movie = json_array_get_object(movies, i);
        int    id    = (int)json_object_get_number(movie, "id");
        const char *m_title = json_object_get_string(movie, "title");

        /* Print “#<id>: <title>” */
        printf("#%d: %s\n", id, m_title ? m_title : "");
    }

    /* Clean up */
    json_value_free(root_val);
}

void print_collections(const char *resp) {
    JSON_Value   *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "ERROR: Invalid JSON\n");
        return;
    }

    JSON_Object  *root_object = json_value_get_object(root_value);
    JSON_Array   *arr = json_object_get_array(root_object, "collections");
    if (!arr) {
        fprintf(stderr, "ERROR: No \"collections\" array found\n");
        json_value_free(root_value);
        return;
    }

    size_t count = json_array_get_count(arr);
    for (size_t i = 0; i < count; ++i) {
        JSON_Object *coll = json_array_get_object(arr, i);
        if (!coll) continue;

        long   id    = (long) json_object_get_number(coll, "id");
        const char *title = json_object_get_string(coll, "title");
        if (title) {
            printf("#%ld: %s\n", id, title);
        }
    }

    json_value_free(root_value);
}

void print_movie_details(const char *resp) {
    /* Parse the JSON text */
    JSON_Value *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "ERROR: Failed to parse JSON\n");
        return;
    }

    /* Get the root object */
    JSON_Object *movie = json_value_get_object(root_value);
    if (!movie) {
        fprintf(stderr, "ERROR: JSON is not an object\n");
        json_value_free(root_value);
        return;
    }

    /* Extract each field (falling back to defaults if missing) */
    const char *title       = json_object_get_string(movie, "title");
    double      year_num    = json_object_get_number(movie, "year");
    const char *description = json_object_get_string(movie, "description");
    const char *rating      = json_object_get_string(movie, "rating");

    if (!title)       title       = "(no title)";
    if (year_num == 0) year_num   = 0;              /* or choose a sentinel */
    if (!description) description = "(no description)";
    if (!rating)      rating      = "(no rating)";

    /* Print in the desired format */
    printf("title: %s\n", title);
    printf("year: %.0f\n", year_num);
    printf("description: %s\n", description);
    printf("rating: %s\n", rating);

    /* Clean up */
    json_value_free(root_value);
}

void print_movies(const char *resp) {
    /* Parse the incoming JSON text */
    JSON_Value *root_value = json_parse_string(resp);
    if (!root_value) {
        fprintf(stderr, "ERROR: Failed to parse JSON\n");
        return;
    }

    /* Get the root object and the "movies" array */
    JSON_Object *root_object = json_value_get_object(root_value);
    JSON_Array  *movies = json_object_get_array(root_object, "movies");
    if (!movies) {
        fprintf(stderr, "ERROR: No \"movies\" array in JSON\n");
        json_value_free(root_value);
        return;
    }

    /* Iterate over each movie */
    size_t count = json_array_get_count(movies);
    for (size_t i = 0; i < count; i++) {
        JSON_Object *movie = json_array_get_object(movies, i);
        if (!movie) continue;

        /* Extract id and title */
        int   id    = (int)json_object_get_number(movie, "id");
        const char *title = json_object_get_string(movie, "title");
        if (!title) title = "(no title)";

        /* Print in the format "#<id> <title>" */
        printf("#%d %s\n", id, title);
    }

    /* Clean up */
    json_value_free(root_value);
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

void print_http_error(int status_code, const char *resp) {
    // 1) Find the JSON body
    const char *body = strchr(resp, '{');
    if (!body) {
        fprintf(stderr, "ERROR: no JSON body found\n");
        return;
    }

    // 2) Locate the "error" key
    const char *key = strstr(body, "\"error\"");
    if (!key) {
        fprintf(stderr, "ERROR: no \"error\" key in JSON\n");
        return;
    }

    // 3) Find the colon after "error"
    const char *colon = strchr(key, ':');
    if (!colon) {
        fprintf(stderr, "ERROR: malformed JSON (no colon after \"error\")\n");
        return;
    }

    // 4) Skip whitespace to the opening quote
    colon++;
    while (*colon == ' ' || *colon == '\t') colon++;
    if (*colon != '\"') {
        fprintf(stderr, "ERROR: malformed JSON (\"error\" value not quoted)\n");
        return;
    }

    // 5) Extract the error message between quotes
    const char *msg_start = colon + 1;
    const char *msg_end   = strchr(msg_start, '\"');
    if (!msg_end) {
        fprintf(stderr, "ERROR: malformed JSON (unterminated string)\n");
        return;
    }
    size_t msg_len = msg_end - msg_start;
    char message[256];
    if (msg_len >= sizeof(message))
        msg_len = sizeof(message) - 1;
    memcpy(message, msg_start, msg_len);
    message[msg_len] = '\0';

    // 6) Print in the requested format
    printf("ERROR: %d %s\n", status_code, message);
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

int setup_conn(void) {
	int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(HOST);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }

	return sockfd;
}

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

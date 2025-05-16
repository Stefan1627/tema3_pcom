#ifndef HELPER_H
#define HELPER_H

#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

#define SA struct sockaddr
#define HOST "63.32.125.183"
#define PORT 8081
#define EXIT 16

int contains_space(const char *str);
char *strip_headers(const char *resp);

int extract_id(const char *resp);
char *extract_token(const char *resp);

void print_collection_details(const char *resp);
void print_collections(const char *resp);
void print_movie_details(const char *resp);
void print_movies(const char *resp);
void print_users(const char *resp);

int get_status(char *resp);
void print_http_error(int status_code, const char *resp);

char *extract_cookie(char *resp);

// Opens a socket and connects to a server
int setup_conn(void);

// Reads a line from stdin (allocates buffer with malloc)
// Returns NULL on EOF
char *helper_readline(void);

// Splits a string into tokens by whitespace; returns NULL-terminated array (malloc’ed)
char **helper_split(const char *str);

// Frees the array returned by helper_split
void helper_free_tokens(char **tokens);

#endif // HELPER_H

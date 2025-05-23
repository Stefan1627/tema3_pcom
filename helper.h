#ifndef HELPER_H
#define HELPER_H
// 324CC Stefan CALMAC

#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

/* Socket address shorthand */
#define SA      struct sockaddr

/* Default server connection parameters */
#define HOST    "63.32.125.183"   /**< Server IP address */
#define PORT    8081              /**< Server port */
#define EXIT    16                /**< Custom exit code for application termination */

/* -------------------------------------------------------------------------- */
/*                              String Utilities                              */
/* -------------------------------------------------------------------------- */

/**
 * Check whether the given string contains any whitespace characters.
 *
 * @param str   Null-terminated string to inspect.
 * @return      1 if any isspace() character is found, 0 otherwise.
 */
int contains_space(const char *str);

/**
 * Strip HTTP headers from a raw response, returning a malloc’d copy of the body.
 *
 * @param resp  Full HTTP response (headers + "\r\n\r\n" + body).
 * @return      Malloc’d string containing only the response body, or NULL on error.
 */
char *strip_headers(const char *resp);

/* -------------------------------------------------------------------------- */
/*                              JSON Utilities                                */
/* -------------------------------------------------------------------------- */

/**
 * Extract an integer "id" field from a JSON-style response string.
 *
 * @param resp  JSON text containing a numeric "id" member.
 * @return      Parsed integer on success, or -1 if missing/malformed.
 */
int extract_id(const char *resp);

/**
 * Parse JSON and extract the "token" string field.
 *
 * @param resp  JSON text containing a "token" member.
 * @return      Malloc’d copy of the token, or NULL on parse/error.
 */
char *extract_token(const char *resp);

/* -------------------------------------------------------------------------- */
/*                         JSON Response Printers                             */
/* -------------------------------------------------------------------------- */

/**
 * Print details of a single collection:
 *   - title
 *   - owner
 *   - numbered list of movies (id and title)
 *
 * @param resp  JSON string containing collection details.
 */
void print_collection_details(const char *resp);

/**
 * Print a list of collections (id and title).
 *
 * @param resp  JSON string containing an array "collections".
 */
void print_collections(const char *resp);

/**
 * Print details of a single movie:
 *   - title
 *   - year
 *   - description
 *   - rating
 *
 * @param resp  JSON string containing movie fields.
 */
void print_movie_details(const char *resp);

/**
 * Print a list of movies (id and title).
 *
 * @param resp  JSON string containing an array "movies".
 */
void print_movies(const char *resp);

/**
 * Print a list of users in the format "#index username:password".
 *
 * @param resp  JSON string containing an array "users".
 */
void print_users(const char *resp);

/* -------------------------------------------------------------------------- */
/*                           HTTP Response Helpers                            */
/* -------------------------------------------------------------------------- */

/**
 * Parse the HTTP status code from a response string.
 *
 * @param resp  Full HTTP response starting with "HTTP/1.1 XXX ...".
 * @return      The integer status code, or 0 if not found.
 */
int get_status(char *resp);

/**
 * Parse and print an error message from an HTTP response:
 *   - status code
 *   - "error" field from JSON body
 *
 * @param status_code  HTTP status code to display.
 * @param resp         Full HTTP response containing a JSON "error" field.
 */
void print_http_error(int status_code, const char *resp);

/* -------------------------------------------------------------------------- */
/*                               Cookie Helper                                */
/* -------------------------------------------------------------------------- */

/**
 * Extract the first "Set-Cookie" header value from an HTTP response.
 *
 * @param resp  Full HTTP response containing "Set-Cookie: name=value".
 * @return      Malloc’d cookie string ("name=value") or NULL if not found.
 */
char *extract_cookie(char *resp);

/* -------------------------------------------------------------------------- */
/*                         Connection & I/O Helpers                          */
/* -------------------------------------------------------------------------- */

/**
 * Open a TCP socket and connect to HOST:PORT.
 * Exits the application if creation or connection fails.
 *
 * @return  Connected socket file descriptor.
 */
int setup_conn(void);

/**
 * Read a line from stdin, allocate buffer with malloc, strip trailing newline.
 *
 * @return  Malloc’d string without newline, or NULL on EOF/error.
 */
char *helper_readline(void);

#endif // HELPER_H

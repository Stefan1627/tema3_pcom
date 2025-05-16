#ifndef REQUESTS_H
#define REQUESTS_H

#include <stddef.h>

// Sends a GET to the given route (e.g., "/movies").
// Returns response body (malloc’d) or NULL on error.
char *request_get(const char *route, int sockfd, const char *extra_hdr, const char *extra_path);

// Sends a POST with JSON body; returns response body.
char *request_post(const char *route, const char *json_body, char *payload, int sockfd, const char *extra_hdr);

// Similarly for PUT, DELETE, etc.
char *request_put(const char *route_base,
                  const char *json_body,
				  const char *payload,
                  int sockfd,
				  const char *movie_id,
                  const char *extra_hdr);
char *request_delete(const char *route_base, const char *username, int sockfd, const char *extra_hdr);

#endif // REQUESTS_H

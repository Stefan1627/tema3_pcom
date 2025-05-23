#ifndef REQUESTS_H
#define REQUESTS_H
// 324CC Stefan CALMAC

#include <stddef.h>

/**
 * @file requests.h
 * @brief Declarations of functions to perform HTTP requests (GET, POST, PUT, DELETE).
 */

/**
 * Perform an HTTP GET request.
 *
 * Constructs and sends a GET to the specified route, optionally appending
 * an extra path segment and additional headers.
 *
 * @param route      Base route (e.g. "/movies").
 * @param sockfd     Connected socket descriptor.
 * @param extra_hdr  Optional extra header string (must include trailing "\r\n"), or NULL.
 * @param extra_path Optional path segment to append to route (e.g. "123"), or NULL.
 * @return           Malloc’d buffer containing the full HTTP response
 *                   (headers + body), or NULL on error.
 */
char *request_get(const char *route,
                  int sockfd,
                  const char *extra_hdr,
                  const char *extra_path);

/**
 * Perform an HTTP POST request with a JSON payload.
 *
 * Sends a POST to the specified route, setting Content-Type and
 * Content-Length headers, including any extra headers, and the JSON body.
 *
 * @param route      Target route (e.g. "/movies").
 * @param json_body  Null-terminated JSON string to send in the request body.
 * @param payload    MIME type of the body (e.g. "application/json").
 * @param sockfd     Connected socket descriptor.
 * @param extra_hdr  Optional extra header string (must include trailing "\r\n"), or NULL.
 * @return           Malloc’d buffer containing the full HTTP response
 *                   (headers + body), or NULL on error.
 */
char *request_post(const char *route,
                   const char *json_body,
                   char *payload,
                   int sockfd,
                   const char *extra_hdr);

/**
 * Perform an HTTP PUT request with a JSON payload.
 *
 * Sends a PUT to route_base/id, setting Content-Type and Content-Length headers,
 * including any extra headers, and the JSON body.
 *
 * @param route_base Base route (e.g. "/movies").
 * @param json_body  Null-terminated JSON string to send in the request body.
 * @param payload    MIME type of the body (e.g. "application/json").
 * @param sockfd     Connected socket descriptor.
 * @param movie_id   Identifier to append to the route (e.g. "123").
 * @param extra_hdr  Optional extra header string (must include trailing "\r\n"), or NULL.
 * @return           Malloc’d buffer containing the full HTTP response
 *                   (headers + body), or NULL on error.
 */
char *request_put(const char *route_base,
                  const char *json_body,
                  const char *payload,
                  int sockfd,
                  const char *movie_id,
                  const char *extra_hdr);

/**
 * Perform an HTTP DELETE request.
 *
 * Sends a DELETE to route_base/id, including any extra headers
 * and a zero-length Content-Length header.
 *
 * @param route_base Base route (e.g. "/movies").
 * @param username   Identifier to delete (e.g. movie or user ID as string).
 * @param sockfd     Connected socket descriptor.
 * @param extra_hdr  Optional extra header string (must include trailing "\r\n"), or NULL.
 * @return           Malloc’d buffer containing the full HTTP response
 *                   (headers + body), or NULL on error.
 */
char *request_delete(const char *route_base,
                     const char *username,
                     int sockfd,
                     const char *extra_hdr);

#endif // REQUESTS_H

// 324CC Stefan CALMAC
#include "requests.h"
#include "helper.h"

/**
 * Perform an HTTP GET request.
 *
 * Constructs and sends a GET request to the specified route,
 * optionally appending an extra path segment and extra headers.
 * Reads the response into a malloc’d buffer and returns it.
 *
 * @param route         Base route (e.g. "/api/movies")
 * @param sockfd        Connected socket descriptor
 * @param extra_hdr     Optional extra header string (must include trailing "\r\n"), or NULL
 * @param extra_path    Optional path segment to append to route (e.g. "123"), or NULL
 * @return              Malloc’d response buffer (headers+body), or NULL on error
 */
char *request_get(const char *route,
                  int sockfd,
                  const char *extra_hdr,
                  const char *extra_path)
{
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", route, extra_path);

    char request[4096];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "%s"
             "Connection: close\r\n"
             "\r\n",
             extra_path ? path : route,
             HOST,
             extra_hdr ? extra_hdr : "");

    /* Send the GET request */
    if (write(sockfd, request, strlen(request)) < 0) {
        perror("write");
        return NULL;
    }

    /* Allocate buffer and read the response */
    char *buf = malloc(8192);
    if (!buf) {
        perror("malloc");
        return NULL;
    }

    ssize_t n = read(sockfd, buf, 8191);
    if (n < 0) {
        perror("read");
        free(buf);
        return NULL;
    }
    buf[n] = '\0';

    return buf;
}

/**
 * Perform an HTTP POST request with JSON payload.
 *
 * Constructs and sends a POST request to the specified route,
 * including Content-Type, Content-Length, optional extra headers,
 * and the JSON body. Reads and returns the full response.
 *
 * @param route         Target route (e.g. "/api/movies")
 * @param json_body     JSON-formatted string to send as the request body
 * @param payload       MIME type of the payload (e.g. "application/json")
 * @param sockfd        Connected socket descriptor
 * @param extra_hdr     Optional extra header string (must include trailing "\r\n"), or NULL
 * @return              Malloc’d response buffer (headers+body), or NULL on error
 */
char *request_post(const char *route,
                   const char *json_body,
                   char *payload,
                   int sockfd,
                   const char *extra_hdr)
{
    char request[4096];
    snprintf(request, sizeof(request),
             "POST %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "%s"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             route, HOST, payload, strlen(json_body),
             extra_hdr ? extra_hdr : "",
             json_body);

    /* Send the POST request */
    if (write(sockfd, request, strlen(request)) < 0) {
        perror("write");
        return NULL;
    }

    /* Allocate buffer and read the response */
    char *buf = malloc(8192);
    if (!buf) {
        perror("malloc");
        return NULL;
    }

    ssize_t n = read(sockfd, buf, 8191);
    if (n <= 0) {
        perror("read");
        free(buf);
        return NULL;
    }
    buf[n] = '\0';

    return buf;
}

/**
 * Perform an HTTP PUT request with JSON payload.
 *
 * Builds a PUT request to route_base/id, sets Content-Type,
 * Content-Length, optional extra headers, and the JSON body.
 * Returns the server’s response in a malloc’d buffer.
 *
 * @param route_base    Base route (e.g. "/api/movies")
 * @param json_body     JSON-formatted string to send as the request body
 * @param payload       MIME type of the payload (e.g. "application/json")
 * @param sockfd        Connected socket descriptor
 * @param movie_id      Identifier to append to the route (e.g. "123")
 * @param extra_hdr     Optional extra header string (must include trailing "\r\n"), or NULL
 * @return              Malloc’d response buffer (headers+body), or NULL on error
 */
char *request_put(const char *route_base,
                  const char *json_body,
                  const char *payload,
                  int sockfd,
                  const char *movie_id,
                  const char *extra_hdr)
{
    /* Build full request path */
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", route_base, movie_id);

    size_t body_len = strlen(json_body);
    char request[4096];
    int req_len = snprintf(request, sizeof(request),
        "PUT %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "%s"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        path, HOST, payload, body_len,
        extra_hdr ? extra_hdr : "",
        json_body);

    if (req_len < 0 || req_len >= (int)sizeof(request)) {
        fprintf(stderr, "request_put: request buffer overflow\n");
        return NULL;
    }

    /* Send the PUT request */
    if (write(sockfd, request, req_len) < 0) {
        perror("write");
        return NULL;
    }

    /* Allocate buffer and read the response */
    char *buf = malloc(8192);
    if (!buf) {
        perror("malloc");
        return NULL;
    }
    ssize_t n = read(sockfd, buf, 8191);
    if (n < 0) {
        perror("read");
        free(buf);
        return NULL;
    }
    buf[n] = '\0';

    return buf;
}

/**
 * Perform an HTTP DELETE request.
 *
 * Constructs and sends a DELETE request to route_base/id,
 * includes optional extra headers, and reads the response.
 *
 * @param route_base    Base route (e.g. "/api/movies")
 * @param id            Identifier to delete (e.g. "123")
 * @param sockfd        Connected socket descriptor
 * @param extra_hdr     Optional extra header string (must include trailing "\r\n"), or NULL
 * @return              Malloc’d response buffer (headers+body), or NULL on error
 */
char *request_delete(const char *route_base,
                     const char *id,
                     int sockfd,
                     const char *extra_hdr)
{
    /* Build full request path */
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", route_base, id);

    char request[4096];
    int req_len = snprintf(request, sizeof(request),
                           "DELETE %s HTTP/1.1\r\n"
                           "Host: %s\r\n"
                           "%s"
                           "Content-Length: 0\r\n"
                           "Connection: close\r\n"
                           "\r\n",
                           path, HOST,
                           extra_hdr ? extra_hdr : "");

    /* Send the DELETE request */
    if (write(sockfd, request, req_len) < 0) {
        perror("write");
        return NULL;
    }

    /* Allocate buffer and read the response */
    char *buf = malloc(8192);
    if (!buf) {
        perror("malloc");
        return NULL;
    }
    ssize_t n = read(sockfd, buf, 8191);
    if (n < 0) {
        perror("read");
        free(buf);
        return NULL;
    }
    buf[n] = '\0';

    return buf;
}

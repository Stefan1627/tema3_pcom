#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include "requests.h"
#include "helper.h"
#include "parson.h"

// You can use libcurl, sockets, or any HTTP library
// Here are stubs:

char *request_get(const char *route) {
    // TODO: build HTTP GET to server_base + route
    // For now, just return a dummy JSON
    return strdup("{ \"status\": \"ok\" }");
}

char *request_post(const char *route,
				   const char *json_body,
				   char *payload,
				   int sockfd,
				   const char *extra_hdr) {
    // TODO: HTTP POST
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

    if (write(sockfd, request, strlen(request)) < 0) {
        perror("write");
        return NULL;
    }

	char *buf = malloc(8192);
	if (!buf) {
        perror("malloc");
        return NULL;
    }

	ssize_t n = read(sockfd, buf, 8191 - 1);
	if (n <= 0) {
        perror("read");
        return NULL;
    }
    buf[n] = '\0';

	return buf;
}

char *request_put(const char *route, const char *json_body) {
    // TODO: HTTP PUT
    return strdup("{ \"status\": \"updated\" }");
}

char *request_delete(const char *route) {
    // TODO: HTTP DELETE
    return strdup("{ \"status\": \"deleted\" }");
}

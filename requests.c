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

	printf("%s\n", request);

	/* Send it */
	if (write(sockfd, request, strlen(request)) < 0)
	{
		perror("write");
		return NULL;
	}

	/* Read the response */
	char *buf = malloc(8192);
	if (!buf)
	{
		perror("malloc");
		return NULL;
	}

	ssize_t n = read(sockfd, buf, 8191);
	if (n < 0)
	{
		perror("read");
		free(buf);
		return NULL;
	}

	/* Null-terminate and return */
	buf[n] = '\0';
	return buf;
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

char *request_delete(const char *route_base,
					 const char *username,
					 int sockfd,
					 const char *extra_hdr)
{
	/* Build the full path: route_base + "/" + username */
	char path[512];
	snprintf(path, sizeof(path), "%s/%s", route_base, username);

	char request[4096];
	int req_len = snprintf(request, sizeof(request),
						   "DELETE %s HTTP/1.1\r\n"
						   "Host: %s\r\n"
						   "%s"
						   "Content-Length: 0\r\n"
						   "Connection: close\r\n"
						   "\r\n",
						   path,
						   HOST,
						   extra_hdr ? extra_hdr : "");

	/* Send the request */
	if (write(sockfd, request, req_len) < 0)
	{
		perror("write");
		return NULL;
	}

	/* Read the response into a malloc’d buffer */
	char *buf = malloc(8192);
	if (!buf)
	{
		perror("malloc");
		return NULL;
	}
	ssize_t n = read(sockfd, buf, 8191);
	if (n < 0)
	{
		perror("read");
		free(buf);
		return NULL;
	}
	buf[n] = '\0';
	return buf;
}

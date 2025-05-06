#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char *request_post(const char *route, const char *json_body) {
    // TODO: HTTP POST
    return strdup("{ \"status\": \"created\" }");
}

char *request_put(const char *route, const char *json_body) {
    // TODO: HTTP PUT
    return strdup("{ \"status\": \"updated\" }");
}

char *request_delete(const char *route) {
    // TODO: HTTP DELETE
    return strdup("{ \"status\": \"deleted\" }");
}

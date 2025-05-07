#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include "commands.h"
#include "requests.h"
#include "parson.h"
#include "helper.h"
#include "routes.h"   // see next section

char *handle_login_admin(char *body, int *sockfd) {
	char *username = helper_readline();
	char *password = helper_readline();

    JSON_Value *root = json_value_init_object();
    JSON_Object *o    = json_value_get_object(root);
    json_object_set_string(o, "username", username);
    json_object_set_string(o, "password", password);
    body = json_serialize_to_string(root);

    char *resp = request_post(ROUTE_ADMIN_LOGIN, body, PAYLOAD_APP_JSON, *sockfd);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
    } else {
		if (has_connection_close(resp)) {
			close(*sockfd);
			*sockfd = -1;
			*sockfd = setup_conn();
		}
        printf("%s\n", resp);
        free(resp);
    }

    // json_free_serialized_string(body);
    json_value_free(root);
    return body;
}

// int handle_add_user(void) {

// }

// Add more handlers: handle_add_movie, handle_list_movies, etc.

char *commands_dispatch(char *cmd, char *body, int *sockfd) {
    if (!cmd) return 0;
    if (strcmp(cmd, "login_admin") == 0) {
        return handle_login_admin(body, sockfd);
    }  

    else if (strcmp(cmd, "exit") == 0) {
        return NULL;
    }

    else {
        printf("Unknown command: %s\n", cmd);
        return 0;
    }
}

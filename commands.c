#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "commands.h"
#include "requests.h"
#include "parson.h"
#include "helper.h"
#include "routes.h"   // see next section

int handle_login_admin(void) {
	char *username = helper_readline();
	char *password = helper_readline();

    JSON_Value *root = json_value_init_object();
    JSON_Object *o    = json_value_get_object(root);
    json_object_set_string(o, "username", username);
    json_object_set_string(o, "password", password);
    char *body = json_serialize_to_string(root);

    char *resp = request_post(ROUTE_ADMIN_LOGIN, body);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
    } else {
        printf("%s\n", resp);
        free(resp);
    }

    json_free_serialized_string(body);
    json_value_free(root);
    return 0;
}

// Add more handlers: handle_add_movie, handle_list_movies, etc.

int commands_dispatch(char *cmd) {
    if (!cmd) return 0;
    if (strcmp(cmd, "login_admin") == 0) {
        return handle_login_admin();
    }
    // else if (strcmp(tokens[0], "add_movie")==0) { … }
    // else if …  

    else if (strcmp(cmd, "exit") == 0) {
        return 1;
    }

    else {
        printf("Unknown command: %s\n", cmd);
        return 0;
    }
}

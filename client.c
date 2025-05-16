#include "client.h"
#include "helper.h"
#include "requests.h"
#include "commands.h"

int client_socket = -1;
char *cookie = NULL;
char *token = NULL;

int commands_dispatch(char *cmd) {
    if (!cmd) return -1;

	if (strcmp(cmd, "exit") == 0) {
		return EXIT;
	}

	close(client_socket);
	client_socket = -1;
	client_socket = setup_conn();

    if (strcmp(cmd, "login_admin") == 0) {
        return handle_login_admin(&cookie, client_socket);
    } else if (strcmp(cmd, "add_user") == 0) {
		return handle_add_user(&cookie, client_socket);
	} else if (strcmp(cmd, "get_users") == 0) {
		return handle_get_users(&cookie, client_socket);
	} else if (strcmp(cmd, "delete_user") == 0) {
		return handle_delete_user(&cookie, client_socket);
	} else if (strcmp(cmd, "login") == 0) {
		return handle_login(&cookie, client_socket);
	} else if (strcmp(cmd, "logout_admin") == 0) {
		return handle_logout_admin(&cookie, client_socket);
	} else if (strcmp(cmd, "logout") == 0) {
		return handle_logout(&cookie, &token, client_socket);
	} else if (strcmp(cmd, "get_access") == 0) {
		return handle_get_access(&cookie, &token, client_socket);
	} else if (strcmp(cmd, "get_movies") == 0) {
		return handle_get_movies(&token, client_socket);
	} else if (strcmp(cmd, "get_movie") == 0) {
		return handle_get_movie(&token, client_socket);
	} else if (strcmp(cmd, "add_movie") == 0) {
		return handle_add_movie(&token, client_socket);
	} else if (strcmp(cmd, "delete_movie") == 0) {
		return handle_delete_movie(&token, client_socket);
	} else if(strcmp(cmd, "update_movie") == 0) {
		return handle_update_movie(&token, client_socket);
	} else if (strcmp(cmd, "get_collections") == 0) {
		return handle_get_collections(&token, client_socket);
	} else if (strcmp(cmd, "get_collection") == 0) {
		return handle_get_collection(&token, client_socket);
	} else if (strcmp(cmd, "add_collection") == 0) {
		return handle_add_collection(&token, client_socket);
	} else if (strcmp(cmd, "delete_collection") == 0) {
		return handle_delete_collection(&token, client_socket);
	} else if (strcmp(cmd, "add_movie_to_collection") == 0) {
		return handle_add_movie_to_collection(&token, client_socket);
	} else if (strcmp(cmd, "delete_movie_from_collection") == 0) {
		return handle_delete_movie_from_collection(&token, client_socket);
	} else {
        printf("Unknown command: %s\n", cmd);
        return 0;
    }
}

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
    client_run();
    client_cleanup();

    return 0;
}

void client_run(void) {
    char *cmd = NULL;
    while ((cmd = helper_readline()) != NULL) {
        int r = commands_dispatch(cmd);
		free(cmd);
		if (r == EXIT)
  			break;
    }
}

void client_cleanup() {
	close(client_socket);
	free(cookie);
	free(token);
}

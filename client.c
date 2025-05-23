// 324CC Stefan CALMAC
#include "helper.h"
#include "requests.h"
#include "commands.h"

/* Global state for the client process */
int   client_socket = -1;   /**< Active socket descriptor, or -1 if closed */
char *cookie        = NULL; /**< Session cookie string (malloc’d), or NULL if not set */
char *token         = NULL; /**< JWT access token string (malloc’d), or NULL if not set */

/**
 * Dispatch a single text command by name.
 * - Closes and reopens the TCP connection on every command to ensure freshness.
 * - Matches the input command string against known commands.
 * - Calls the appropriate handler function, passing pointers to cookie/token and socket.
 *
 * @param cmd  Null-terminated command string (e.g. "login", "get_movies", "exit").
 * @return     Handler-specific return code, or EXIT to signal program termination.
 *             Returns 0 for unrecognized commands.
 */
int commands_dispatch(char *cmd) {
    if (!cmd) return -1;

    /* Special built-in: exit command */
    if (strcmp(cmd, "exit") == 0) {
        return EXIT;
    }

    /* Re-establish the connection for each command */
    close(client_socket);
    client_socket = -1;
    client_socket = setup_conn();

    /* Match and invoke the corresponding handler */
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
    } else if (strcmp(cmd, "update_movie") == 0) {
        return handle_update_movie(&token, client_socket);
    } else if (strcmp(cmd, "get_collections") == 0) {
        return handle_get_collections(&token, client_socket);
    } else if (strcmp(cmd, "get_collection") == 0) {
        return handle_get_collection(&token, client_socket);
    } else if (strcmp(cmd, "add_collection") == 0) {
        return handle_add_collection(&token, client_socket);
    } else if (strcmp(cmd, "delete_collection") == 0) {
        /* 'false' indicates user will be prompted for the collection ID */
        return handle_delete_collection(&token, client_socket, false, NULL);
    } else if (strcmp(cmd, "add_movie_to_collection") == 0) {
        return handle_add_movie_to_collection(&token, client_socket);
    } else if (strcmp(cmd, "delete_movie_from_collection") == 0) {
        return handle_delete_movie_from_collection(&token, client_socket);
    } else {
        /* Unknown commands are simply echoed back */
        printf("Unknown command: %s\n", cmd);
        return 0;
    }
}

/**
 * Main client loop: read commands from stdin until EOF or 'exit' is entered.
 * - Uses helper_readline() to get each command string.
 * - Frees the command buffer after dispatch.
 * - Breaks out of the loop when commands_dispatch returns EXIT.
 */
void client_run(void) {
    char *cmd = NULL;
    while ((cmd = helper_readline()) != NULL) {
        int r = commands_dispatch(cmd);
        free(cmd);
        if (r == EXIT)
            break;
    }
}

/**
 * Clean up global client state before exiting:
 * - Close any open socket.
 * - Free malloc’d cookie and token strings if set.
 */
void client_cleanup(void) {
    close(client_socket);
    free(cookie);
    free(token);
}

/**
 * Program entry point:
 * - Disable stdout buffering for immediate feedback.
 * - Enter the client command loop.
 * - Perform cleanup on exit.
 */
int main(int argc, char *argv[]) {
    /* Ensure prompt output appears immediately */
    setvbuf(stdout, NULL, _IONBF, 0);

    client_run();
    client_cleanup();

    return 0;
}

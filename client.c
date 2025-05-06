#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"
#include "helper.h"
#include "requests.h"
#include "commands.h"

int main(int argc, char *argv[]) {
    client_init(HOST, PORT);
    client_run();
    client_cleanup();

    return 0;
}

void client_init(const char *server_host, int server_port) {
    // TODO: store host/port, initialize helper or requests modules
    printf("Connecting to %s:%d...\n", server_host, server_port);
}

void client_run(void) {
    char *cmd = NULL;
    while ((cmd = helper_readline()) != NULL) {
        // TODO: parse command, call appropriate request
		printf("Received line: %s\n", cmd);
		if (commands_dispatch(cmd) != 0) {
			free(cmd);
			break;
		}
        free(cmd);
    }
}

void client_cleanup(void) {
    // TODO: cleanup any global state
	printf("Disconnecting...\n");
}

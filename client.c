#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client.h"
#include "helper.h"
#include "requests.h"
#include "commands.h"

int main(int argc, char *argv[]) {
    int sockfd;

	sockfd = setup_conn();

    client_run(&sockfd);
    client_cleanup(sockfd);

    return 0;
}

int setup_conn(void) {
	int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(HOST);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else {
        printf("connected to the server..\n");
	}

	return sockfd;
}

void client_run(int *sockfd) {
    char *cmd = NULL;
	char *body = NULL;
    while ((cmd = helper_readline()) != NULL) {
        // TODO: parse command, call appropriate request
		printf("Received line: %s\n", cmd);
		if (commands_dispatch(cmd, body, sockfd) == NULL) {
			free(cmd);
			break;
		}
        free(cmd);
		free(body);
    }
}

void client_cleanup(int sockfd) {
    // TODO: cleanup any global state
	printf("Disconnecting...\n");
	close(sockfd);
}

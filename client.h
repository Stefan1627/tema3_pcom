#ifndef CLIENT_H
#define CLIENT_H

#define SA struct sockaddr

// Opens a socket and connects to a server
int setup_conn(void);

// Reads a line from stdin and dispatches it
void client_run(int *sockfd);

// Cleans up any allocated resources
void client_cleanup(int sockfd);

#endif // CLIENT_H

#ifndef CLIENT_H
#define CLIENT_H

#define HOST "63.32.125.183"
#define PORT 8081

// Initializes the client (e.g., reads config, sets up base URL)
void client_init(const char *server_host, int server_port);

// Reads a line from stdin and dispatches it
void client_run(void);

// Cleans up any allocated resources
void client_cleanup(void);

#endif // CLIENT_H

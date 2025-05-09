#ifndef COMMANDS_H
#define COMMANDS_H

// Dispatch a user-typed command.  tokens[0] is the command name.
// Returns 0 to continue, or nonzero to exit the client.
int commands_dispatch(char *cmd, char **cookie, char **token, int *sockfd);

#endif // COMMANDS_H

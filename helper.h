#ifndef HELPER_H
#define HELPER_H

#include <stddef.h>

// Reads a line from stdin (allocates buffer with malloc)
// Returns NULL on EOF
char *helper_readline(void);

// Splits a string into tokens by whitespace; returns NULL-terminated array (malloc’ed)
char **helper_split(const char *str);

// Frees the array returned by helper_split
void helper_free_tokens(char **tokens);

#endif // HELPER_H

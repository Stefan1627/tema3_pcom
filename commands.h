#ifndef COMMANDS_H
#define COMMANDS_H
// 324CC Stefan CALMAC

#include <stdbool.h>

#define HDR_COOKIE_SZ 256   // Maximum size for HTTP header strings
#define CODE_SZ 4           // Size for HTTP status code string

/* -------------------------------------------------------------------------- */
/*                        Movie Collection Handlers                           */
/* -------------------------------------------------------------------------- */

/**
 * Prompt the user for a collection ID and movie ID, then add the movie
 * to the specified collection via POST request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_add_movie_to_collection(char **token, int sockfd);

/**
 * Prompt the user for a collection ID and movie ID, then remove the movie
 * from the specified collection via DELETE request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_delete_movie_from_collection(char **token, int sockfd);

/**
 * Prompt the user for a new collection title and initial movie IDs,
 * create the collection via POST, and add each movie to it.
 * Rolls back (deletes) the collection if any add fails.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_add_collection(char **token, int sockfd);

/**
 * Prompt the user for a collection ID and retrieve its details
 * via GET request, then print them.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_get_collection(char **token, int sockfd);

/**
 * Retrieve and print the list of all collections via GET request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_get_collections(char **token, int sockfd);

/**
 * Delete a collection by ID via DELETE request.
 * If coming_from_add is true, skips the user prompt for ID.
 *
 * @param token             Pointer to the JWT access token string.
 * @param sockfd            Active socket descriptor for HTTP communication.
 * @param coming_from_add   Whether this deletion follows a failed add.
 * @param id                Collection ID to delete.
 * @return                  0 on success, negative on error.
 */
int handle_delete_collection(char **token, int sockfd,
							 bool coming_from_add, char *id);


/* -------------------------------------------------------------------------- */
/*                            Movie Handlers                                  */
/* -------------------------------------------------------------------------- */

/**
 * Prompt the user for movie details (title, year, description, rating),
 * validate inputs, and add the movie via POST request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_add_movie(char **token, int sockfd);

/**
 * Prompt the user for a movie ID and retrieve its details
 * via GET request, then print them.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_get_movie(char **token, int sockfd);

/**
 * Retrieve and print the list of all movies via GET request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_get_movies(char **token, int sockfd);

/**
 * Prompt the user for movie ID and updated details,
 * then update the movie via PUT request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_update_movie(char **token, int sockfd);

/**
 * Prompt the user for a movie ID and delete the movie
 * via DELETE request.
 *
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_delete_movie(char **token, int sockfd);


/* -------------------------------------------------------------------------- */
/*                       Authentication & Access                              */
/* -------------------------------------------------------------------------- */

/**
 * Exchange the session cookie for a JWT access token via GET request.
 *
 * @param cookie  Pointer to the session cookie string.
 * @param token   Out parameter for storing the new JWT token.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_get_access(char **cookie, char **token, int sockfd);

/**
 * Log out the current user by sending a GET request and
 * clearing both cookie and token on success.
 *
 * @param cookie  Pointer to the session cookie string.
 * @param token   Pointer to the JWT access token string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_logout(char **cookie, char **token, int sockfd);

/**
 * Log out the current admin by sending a GET request and
 * clearing the admin cookie on success.
 *
 * @param cookie  Pointer to the admin session cookie string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_logout_admin(char **cookie, int sockfd);

/**
 * Prompt for user credentials (admin_username, username, password),
 * perform login via POST, and store the session cookie.
 *
 * @param cookie  Out parameter for storing the new session cookie.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_login(char **cookie, int sockfd);

/**
 * Prompt for admin credentials (username, password),
 * perform admin login via POST, and store the session cookie.
 *
 * @param cookie  Out parameter for storing the new admin cookie.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_login_admin(char **cookie, int sockfd);


/* -------------------------------------------------------------------------- */
/*                            User Management                                 */
/* -------------------------------------------------------------------------- */

/**
 * Prompt for new user credentials (username, password),
 * validate inputs, and add the user via POST request.
 *
 * @param cookie  Pointer to the session cookie string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_add_user(char **cookie, int sockfd);

/**
 * Retrieve and print the list of all users via GET request.
 *
 * @param cookie  Pointer to the session cookie string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_get_users(char **cookie, int sockfd);

/**
 * Prompt for a username and delete that user via DELETE request.
 *
 * @param cookie  Pointer to the session cookie string.
 * @param sockfd  Active socket descriptor for HTTP communication.
 * @return        0 on success, negative on error.
 */
int handle_delete_user(char **cookie, int sockfd);

#endif // COMMANDS_H

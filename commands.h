#ifndef COMMANDS_H
#define COMMANDS_H

#define HDR_COOKIE_SZ 256
#define CODE_SZ 4

/* Movie collection handlers */
int handle_add_movie_to_collection(char **token, int sockfd);
int handle_delete_movie_from_collection(char **token, int sockfd);
int handle_add_collection(char **token, int sockfd);
int handle_get_collection(char **token, int sockfd);
int handle_get_collections(char **token, int sockfd);
int handle_delete_collection(char **token, int sockfd);

/* Movie handlers */
int handle_add_movie(char **token, int sockfd);
int handle_get_movie(char **token, int sockfd);
int handle_get_movies(char **token, int sockfd);
int handle_update_movie(char **token, int sockfd);
int handle_delete_movie(char **token, int sockfd);

/* Authentication & access */
int handle_get_access(char **cookie, char **token, int sockfd);
int handle_logout(char **cookie, char **token, int sockfd);
int handle_logout_admin(char **cookie, int sockfd);
int handle_login(char **cookie, int sockfd);
int handle_login_admin(char **cookie, int sockfd);

/* User management */
int handle_add_user(char **cookie, int sockfd);
int handle_get_users(char **cookie, int sockfd);
int handle_delete_user(char **cookie, int sockfd);

#endif // COMMANDS_H

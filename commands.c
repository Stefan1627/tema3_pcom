#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "client.h"
#include "commands.h"
#include "requests.h"
#include "parson.h"
#include "helper.h"
#include "routes.h"

#define HDR_COOKIE_SZ 256

int handle_logout(char **cookie, int *sockfd) {
	if (cookie == NULL || sockfd == NULL) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_USER_LOGOUT, *sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 200) {
			free(*cookie);
			*cookie = NULL;
			printf("SUCCESS: Utilizator delogat\n");
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_logout_admin(char **cookie, int *sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_ADMIN_LOGOUT, *sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 200) {
			free(*cookie);
			*cookie = NULL;
			printf("SUCCESS: Admin delogat\n");
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_login(char **cookie, int *sockfd) {
	char *admin_username = helper_readline();
	char *username = helper_readline();
	char *password = helper_readline();

	if (strlen(admin_username) == 0 || strlen(username) == 0 || strlen(password) == 0) {
		printf("Error: Username or password or admin_username is empty.\n");
		return -1;
	}

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "admin_username", admin_username);
	json_object_set_string(o, "username", username);
	json_object_set_string(o, "password", password);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_USER_LOGIN, body, PAYLOAD_APP_JSON, *sockfd, NULL);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 200) {
			printf("SUCCESS: Autentificare reușită\n");
        	*cookie = strdup(extract_cookie(resp));
		}
        free(resp);
    }

	return 0;
}

int handle_delete_user(char **cookie, int *sockfd) {
	char *username = helper_readline();

	if (username == NULL) {
		fprintf(stderr, "Error reading username\n");
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_delete(ROUTE_MANAGE_USER, username, *sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 200) {
			printf("SUCCESS: Utilizator șters\n");
		}
        free(resp);
    }
	return 0;
}

int handle_get_users(char **cookie, int *sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_MANAGE_USER, *sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 200) {
			printf("SUCCESS: Lista utilizatorilor\n");
			char *body = strip_headers(resp);
			if (body) {
    			print_users(body);
    			free(body);
			}
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_add_user(char **cookie, int *sockfd) {
	char *username = helper_readline();
	char *password = helper_readline();

	if (strlen(username) == 0 || strlen(password) == 0) {
		printf("ERROR: username and password must be provided\n");
		return -1;
	}

	JSON_Value *root = json_value_init_object();
    JSON_Object *o    = json_value_get_object(root);
    json_object_set_string(o, "username", username);
    json_object_set_string(o, "password", password);
    char *body = json_serialize_to_string(root);

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_post(ROUTE_MANAGE_USER, body, PAYLOAD_APP_JSON, *sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 201) {
			printf("SUCCESS: Utilizator adăugat cu succes\n");
		}
        free(resp);
    }

	free(hdr_cookie);
	json_free_serialized_string(body);
	json_value_free(root);
	return 0;
}

int handle_login_admin(char **cookie, int *sockfd) {
	if (*cookie) {
		printf("Already connected with an account\n");
		return 0;
	}

	char *username = helper_readline();
	char *password = helper_readline();

    JSON_Value *root = json_value_init_object();
    JSON_Object *o    = json_value_get_object(root);
    json_object_set_string(o, "username", username);
    json_object_set_string(o, "password", password);
    char *body = json_serialize_to_string(root);

    char *resp = request_post(ROUTE_ADMIN_LOGIN, body, PAYLOAD_APP_JSON, *sockfd, NULL);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		if (status == 200) {
			printf("SUCCESS: Admin autentificat cu succes\n");
        	*cookie = strdup(extract_cookie(resp));
		}
        free(resp);
    }

    json_free_serialized_string(body);
    json_value_free(root);
    return 0;
}

// Add more handlers: handle_add_movie, handle_list_movies, etc.

int commands_dispatch(char *cmd, char **cookie, char **token, int *sockfd) {
    if (!cmd) return -1;

	if (strcmp(cmd, "exit") == 0) {
		return EXIT;
	}

	close(*sockfd);
	*sockfd = -1;
	*sockfd = setup_conn();

    if (strcmp(cmd, "login_admin") == 0) {
        return handle_login_admin(cookie, sockfd);
    } else if (strcmp(cmd, "add_user") == 0) {
		return handle_add_user(cookie, sockfd);
	} else if (strcmp(cmd, "get_users") == 0) {
		return handle_get_users(cookie, sockfd);
	} else if (strcmp(cmd, "delete_user") == 0) {
		return handle_delete_user(cookie, sockfd);
	} else if (strcmp(cmd, "login") == 0) {
		return handle_login(cookie, sockfd);
	} else if (strcmp(cmd, "logout_admin") == 0) {
		return handle_logout_admin(cookie, sockfd);
	} else if (strcmp(cmd, "logout") == 0) {
		return handle_logout(cookie, sockfd);
	} else {
        printf("Unknown command: %s\n", cmd);
        return 0;
    }
}

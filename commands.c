#include "client.h"
#include "commands.h"
#include "requests.h"
#include "parson.h"
#include "helper.h"
#include "routes.h"

int add_movie_to_collection(char **cookie, char **token, int *sockfd, int collection_id, int movie_id) {
	close(*sockfd);
	*sockfd = -1;
	*sockfd = setup_conn();
	
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_number(o, "id", movie_id);
	char *body = json_serialize_to_string(root);

	char path[512];
	snprintf(path, sizeof(path), "%s/%d/movies", ROUTE_MANAGE_COLLECTIONS, collection_id);

	char *resp = request_post(path, body, PAYLOAD_APP_JSON, *sockfd, hdr_cookie);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] != '2') {
			print_http_error(status, resp);
			free(resp);
			return -2;
		}
        free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return 0;
	}
}

int handle_add_movie_to_collection(char **cookie, char **token, int sockfd) {
	printf("collection_id=");
	int collection_id = atoi(helper_readline());

	printf("movie_id=");
	int movie_id = atoi(helper_readline());

	int res = add_movie_to_collection(cookie, token, &sockfd, collection_id, movie_id);
	if (res > -1) {
		printf("SUCCESS: Film adauga la colectie");
	}
	return 0;
}

int handle_delete_movie_from_collection(char **cookie, char **token, int sockfd) {
	printf("collection_id=");
	int collection_id = atoi(helper_readline());

	printf("movie_id=");
	char *movie_id = helper_readline();

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	char path[512];
	snprintf(path, sizeof(path), "%s/%d/movies", ROUTE_MANAGE_COLLECTIONS, collection_id);

	char *resp = request_delete(path, movie_id, sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Film șters din colecție\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }
	return 0;
}

int handle_delete_collection(char **cookie, char **token, int sockfd) {
	printf("id=");
	char *id = helper_readline();

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	char *resp = request_delete(ROUTE_MANAGE_COLLECTIONS, id, sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Colecție ștearsă\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }
	return 0;
}

int handle_add_collection(char **cookie, char **token, int sockfd) {
	printf("title=");
	char *title = helper_readline();

	printf("num_movies=");
	int num_movies = atoi(helper_readline());

	int *ids = malloc(num_movies * sizeof(int));
	for (int i = 0; i < num_movies; i++) {
		printf("movie_id[%d]=", i);
		ids[i] = atoi(helper_readline());
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "title", title);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_MANAGE_COLLECTIONS, body, PAYLOAD_APP_JSON, sockfd, hdr_cookie);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int res = 0;
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			int id = extract_id(resp);
			for (int i = 0; i < num_movies; i++) {
				res = add_movie_to_collection(cookie, token, &sockfd, id, ids[i]);
				if (res < 0) {
					break;
				}
			}

			if (res >= 0) {
				printf("SUCCESS: Colectie creata\n");
			}
		} else {
			print_http_error(status, resp);
		}
		free(resp);
    }

	json_free_serialized_string(body);
	json_value_free(root);
	return 0;
}

int handle_get_collection(char **cookie, char **token, int sockfd) {
	printf("id=");
	char *id = helper_readline();

	if (id == NULL) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

		char *resp = request_get(ROUTE_MANAGE_COLLECTIONS, sockfd, hdr_cookie, id);
		if (!resp) {
			fprintf(stderr, "Error: no response\n");
			free(resp);
			free(hdr_cookie);
			return -1;
		} else {
			int status = get_status(resp);
			char str[CODE_SZ];
			sprintf(str, "%d", status);
			if (str[0] == '2') {
				printf("SUCCESS: Detalii colectie\n");
				char *body = strip_headers(resp);
				if (body) {
					print_collection_details(body);
					free(body);
				}
			} else {
				print_http_error(status, resp);
			}
			free(resp);
		}
	
		free(hdr_cookie);
		return 0;
}

int handle_get_collections(char **cookie, char **token, int sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	char *resp = request_get(ROUTE_MANAGE_COLLECTIONS, sockfd, hdr_cookie, NULL);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Lista colecțiilor\n");
			char *body = strip_headers(resp);
			if (body) {
    			print_collections(body);
    			free(body);
			}
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_update_movie(char **cookie, char **token, int sockfd) {
	printf("id=");
	char *id = helper_readline();

	printf("title=");
	char *title = helper_readline();

	printf("year=");
	int year = atoi(helper_readline());

	printf("description=");
	char *description = helper_readline();

	printf("rating=");
	float rating = atof(helper_readline());

	if (id == NULL) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "title", title);
	json_object_set_number(o, "year", year);
	json_object_set_string(o, "description", description);
	json_object_set_number(o, "rating", rating);
	char *body = json_serialize_to_string(root);

	char *resp = request_put(ROUTE_MANAGE_MOVIE, body, PAYLOAD_APP_JSON, sockfd, id, hdr_cookie);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Film actualizat\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	return 0;
}

int handle_delete_movie(char **cookie, char **token, int sockfd) {
	printf("id=");
	char *id = helper_readline();

	if (id == NULL) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	char *resp = request_delete(ROUTE_MANAGE_MOVIE, id, sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Film șters cu succes\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }
	return 0;
}

int handle_add_movie(char **cookie, char **token, int sockfd) {
	printf("title=");
	char *title = helper_readline();

	printf("year=");
	int year = atoi(helper_readline());

	printf("description=");
	char *description = helper_readline();

	printf("rating=");
	float rating = atof(helper_readline());

	if (rating >= 10.0) {
		printf("ERROR: Rating must be between 0.0 and 10.0\n");
		return -1;
	}

	if (strlen(title) == 0 || strlen(description) == 0) {
		printf("Error: All fields are required.\n");
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "title", title);
	json_object_set_number(o, "year", year);
	json_object_set_string(o, "description", description);
	json_object_set_number(o, "rating", rating);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_MANAGE_MOVIE, body, PAYLOAD_APP_JSON, sockfd, hdr_cookie);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Film adăugat\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	return 0;
}

int handle_get_movie(char **cookie, char **token, int sockfd) {
	printf("id=");
	char *movie_id = helper_readline();

	if (movie_id == NULL) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

		char *resp = request_get(ROUTE_MANAGE_MOVIE, sockfd, hdr_cookie, movie_id);
		if (!resp) {
			fprintf(stderr, "Error: no response\n");
			free(resp);
			free(hdr_cookie);
			return -1;
		} else {
			int status = get_status(resp);
			char str[CODE_SZ];
			sprintf(str, "%d", status);
			if (str[0] == '2') {
				printf("SUCCESS: Detalii film\n");
				char *body = strip_headers(resp);
				if (body) {
					print_movie_details(body);
					free(body);
				}
			} else {
				print_http_error(status, resp);
			}
			free(resp);
		}
	
		free(hdr_cookie);
		return 0;
}

int handle_get_movies(char **cookie, char **token, int sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Authorization: Bearer %s\r\n", *token);

	char *resp = request_get(ROUTE_MANAGE_MOVIE, sockfd, hdr_cookie, NULL);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Lista filmelor\n");
			char *body = strip_headers(resp);
			if (body) {
    			print_movies(body);
    			free(body);
			}
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_get_access(char **cookie, char **token, int sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_GET_ACCESS, sockfd, hdr_cookie, NULL);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Token JWT primit\n");
			char *body = strip_headers(resp);
			if (body) {
    			*token = strdup(extract_token(body));
    			free(body);
			}
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_logout(char **cookie, char **token, int sockfd) {
	if (cookie == NULL || sockfd <= 0) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_USER_LOGOUT, sockfd, hdr_cookie, NULL);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			free(*token);
			*token = NULL;
			free(*cookie);
			*cookie = NULL;
			printf("SUCCESS: Utilizator delogat\n");
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_logout_admin(char **cookie, int sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_ADMIN_LOGOUT, sockfd, hdr_cookie, NULL);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			free(*cookie);
			*cookie = NULL;
			printf("SUCCESS: Admin delogat\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_login(char **cookie, int sockfd) {
	printf("admin_username=");
	char *admin_username = helper_readline();

	printf("username=");
	char *username = helper_readline();

	printf("password=");
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

	char *resp = request_post(ROUTE_USER_LOGIN, body, PAYLOAD_APP_JSON, sockfd, NULL);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Autentificare reușită\n");
        	*cookie = strdup(extract_cookie(resp));
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	return 0;
}

int handle_delete_user(char **cookie, int sockfd) {
	printf("username=");
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

	char *resp = request_delete(ROUTE_MANAGE_USER, username, sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Utilizator șters\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }
	return 0;
}

int handle_get_users(char **cookie, int sockfd) {
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		return -1;
	}

	snprintf(hdr_cookie, HDR_COOKIE_SZ,
		"Cookie: %s\r\n", *cookie);

	char *resp = request_get(ROUTE_MANAGE_USER, sockfd, hdr_cookie, NULL);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Lista utilizatorilor\n");
			char *body = strip_headers(resp);
			if (body) {
    			print_users(body);
    			free(body);
			}
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	free(hdr_cookie);
	return 0;
}

int handle_add_user(char **cookie, int sockfd) {
	printf("username=");
	char *username = helper_readline();

	printf("password=");
	char *password = helper_readline();

	if (strlen(username) == 0 || strlen(password) == 0) {
		printf("ERROR: username and password must be provided\n");
		return -1;
	}

	if (contains_space(username)) {
        printf("ERROR: username must not contain spaces\n");
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

	char *resp = request_post(ROUTE_MANAGE_USER, body, PAYLOAD_APP_JSON, sockfd, hdr_cookie);
	if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Utilizator adăugat cu succes\n");
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

	free(hdr_cookie);
	json_free_serialized_string(body);
	json_value_free(root);
	return 0;
}

int handle_login_admin(char **cookie, int sockfd) {
	if (*cookie) {
		printf("Already connected with an account\n");
		return 0;
	}

	printf("username=");
	char *username = helper_readline();

	printf("password=");
	char *password = helper_readline();

    JSON_Value *root = json_value_init_object();
    JSON_Object *o    = json_value_get_object(root);
    json_object_set_string(o, "username", username);
    json_object_set_string(o, "password", password);
    char *body = json_serialize_to_string(root);

    char *resp = request_post(ROUTE_ADMIN_LOGIN, body, PAYLOAD_APP_JSON, sockfd, NULL);
    if (!resp) {
        fprintf(stderr, "Error: no response\n");
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
    } else {
		int status = get_status(resp);
		char str[CODE_SZ];
		sprintf(str, "%d", status);
		if (str[0] == '2') {
			printf("SUCCESS: Admin autentificat cu succes\n");
        	*cookie = strdup(extract_cookie(resp));
		} else {
			print_http_error(status, resp);
		}
        free(resp);
    }

    json_free_serialized_string(body);
    json_value_free(root);
    return 0;
}

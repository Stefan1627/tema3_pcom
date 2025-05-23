// 324CC Stefan CALMAC
#include "commands.h"
#include "requests.h"
#include "parson.h"
#include "helper.h"
#include "routes.h"

/* Sends a POST request to add the specified movie to the given collection.
 * Re-establishes the connection if necessary and attaches the JWT token header.
 * Returns 0 on success, -1 on no response, -2 for HTTP errors.
 */
int add_movie_to_collection(char **token, int *sockfd, int collection_id, int movie_id)
{
	close(*sockfd);
	*sockfd = -1;
	*sockfd = setup_conn();

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}

	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_number(o, "id", movie_id);
	char *body = json_serialize_to_string(root);

	char path[512];
	snprintf(path, sizeof(path), "%s/%d/movies",
			 ROUTE_MANAGE_COLLECTIONS, collection_id);

	char *resp = request_post(path, body, PAYLOAD_APP_JSON, *sockfd, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
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
		free(hdr_token);
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return 0;
	}
}

/* Prompts for collection and movie IDs, checks authorization,
 * validates inputs, and calls add_movie_to_collection.
 * Prints a success message if the addition succeeds.
 */
int handle_add_movie_to_collection(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("collection_id=");
	char *temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: collection_id is required\n");
		return -1;
	}
	for (char *p = temp; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: collection_id must be a number\n");
			return -1;
		}
	}
	int collection_id = atoi(temp);

	printf("movie_id=");
	temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: movie_id is required\n");
		return -1;
	}
	for (char *p = temp; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: movie_id must be a number\n");
			return -1;
		}
	}
	int movie_id = atoi(temp);

	int res = add_movie_to_collection(token, &sockfd, collection_id, movie_id);
	if (res > -1) {
		printf("SUCCESS: Film adauga la colectie");
	}
	return 0;
}

/* Prompts for collection and movie IDs, validates inputs, then sends a
 * DELETE request to remove the movie. Checks HTTP response
 * and reports success or failure.
 */
int handle_delete_movie_from_collection(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("collection_id=");
	char *temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: collection_id is required\n");
		return -1;
	}
	for (char *p = temp; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: collection_id must be a number\n");
			return -1;
		}
	}
	int collection_id = atoi(temp);

	printf("movie_id=");
	char *movie_id = helper_readline();
	if (!movie_id || strlen(movie_id) == 0) {
		printf("ERROR: movie_id is required\n");
		return -1;
	}
	for (char *p = movie_id; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: movie_id must be a number\n");
			return -1;
		}
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}

	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char path[512];
	snprintf(path, sizeof(path), "%s/%d/movies",
			 ROUTE_MANAGE_COLLECTIONS, collection_id);

	char *resp = request_delete(path, movie_id, sockfd, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			printf("SUCCESS: Film șters din colecție\n");
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	free(hdr_token);
	return 0;
}

/* Deletes a collection by ID, optionally bypassing the
 * prompt if coming from add_collection.
 * Validates input, cleans up the auth header, and handles HTTP response.
 */
int handle_delete_collection(char **token, int sockfd,
							 bool coming_from_add, char *id)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	if (!coming_from_add) {
		printf("id=");
		char *temp = helper_readline();
		if (!temp || strlen(temp) == 0) {
			printf("ERROR: id is required\n");
			return -1;
		}
		for (char *p = temp; *p; ++p) {
			if (!isdigit((unsigned char)*p)) {
				printf("ERROR: id must be a number\n");
				return -1;
			}
		}
		id = temp;
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}

	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_delete(ROUTE_MANAGE_COLLECTIONS, id,
								sockfd, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			if (!coming_from_add) {
				printf("SUCCESS: Colecție ștearsă\n");
			}
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	free(hdr_token);
	return 0;
}

/* Creates a new collection with the given title and initial movies.
 * Validates all inputs. On successful creation, adds each movie and rolls
 * back if any addition fails.
 */
int handle_add_collection(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("title=");
	char *title = helper_readline();
	if (!title || strlen(title) == 0) {
		printf("ERROR: title is required\n");
		return -1;
	}

	printf("num_movies=");
	char *temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: num_movies is required\n");
		return -1;
	}
	for (char *p = temp; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: num_movies must be a number\n");
			return -1;
		}
	}
	int num_movies = atoi(temp);

	int *ids = malloc(num_movies * sizeof(int));
	for (int i = 0; i < num_movies; i++) {
		printf("movie_id[%d]=", i);
		temp = helper_readline();
		if (!temp || strlen(temp) == 0) {
			printf("ERROR: movie_id[%d] is required\n", i);
			return -1;
		}
		for (char *p = temp; *p; ++p) {
			if (!isdigit((unsigned char)*p)) {
				printf("ERROR: movie_id[%d] must be a number\n", i);
				return -1;
			}
		}
		ids[i] = atoi(temp);
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}

	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "title", title);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_MANAGE_COLLECTIONS, body,
							  PAYLOAD_APP_JSON, sockfd, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int res = 0;
		int status = get_status(resp);
		if (status / 100 == 2) {
			int id = extract_id(resp);
			for (int i = 0; i < num_movies; i++) {
				res = add_movie_to_collection(token, &sockfd, id, ids[i]);
				if (res < 0) {
					break;
				}
			}

			if (res >= 0) {
				printf("SUCCESS: Colectie creata\n");
			} else {
				char str[24];
				sprintf(str, "%d", id);
				close(sockfd);
				sockfd = -1;
				sockfd = setup_conn();
				handle_delete_collection(token, sockfd, true, str);
			}
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	free(ids);
	free(hdr_token);
	json_free_serialized_string(body);
	json_value_free(root);
	return 0;
}

/* Retrieves details for a single collection and prints them.
 * Validates input and prompts for collection ID.
 */
int handle_get_collection(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("id=");
	char *id = helper_readline();
	if (!id || strlen(id) == 0) {
		printf("ERROR: id is required\n");
		return -1;
	}
	for (char *p = id; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: id must be a number\n");
			return -1;
		}
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}

	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_get(ROUTE_MANAGE_COLLECTIONS, sockfd, hdr_token, id);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
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

	free(hdr_token);
	return 0;
}

/* Retrieves and prints the list of all collections.
 * Requires no extra input.
 */
int handle_get_collections(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}

	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_get(ROUTE_MANAGE_COLLECTIONS, sockfd,
							 hdr_token, NULL);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
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

	free(hdr_token);
	return 0;
}

/* Updates an existing movie's details by sending a PUT request.
 * Prompts for ID, title, year, description, and rating with validation.
 */
int handle_update_movie(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("id=");
	char *id = helper_readline();
	if (!id || strlen(id) == 0) {
		printf("ERROR: id is required\n");
		return -1;
	}
	for (char *p = id; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: id must be a number\n");
			return -1;
		}
	}

	printf("title=");
	char *title = helper_readline();
	if (!title || strlen(title) == 0) {
		printf("ERROR: title is required\n");
		return -1;
	}

	printf("year=");
	char *temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: year is required\n");
		return -1;
	}
	for (char *p = temp; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: year must be a number\n");
			return -1;
		}
	}
	int year = atoi(temp);

	printf("description=");
	char *description = helper_readline();
	if (!description || strlen(description) == 0) {
		printf("ERROR: description is required\n");
		return -1;
	}

	printf("rating=");
	temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: rating is required\n");
		return -1;
	}
	{   int dot_count = 0;
	    for (char *p = temp; *p; ++p) {
	        if (*p == '.') {
	            if (++dot_count > 1) {
	                printf("ERROR: rating must be a valid number\n");
	                return -1;
	            }
	        } else if (!isdigit((unsigned char)*p)) {
	            printf("ERROR: rating must be a number\n");
	            return -1;
	        }
	    }
	}
	float rating = atof(temp);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "title", title);
	json_object_set_number(o, "year", year);
	json_object_set_string(o, "description", description);
	json_object_set_number(o, "rating", rating);
	char *body = json_serialize_to_string(root);

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_put(ROUTE_MANAGE_MOVIE, body, PAYLOAD_APP_JSON,
							 sockfd, id, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			printf("SUCCESS: Film actualizat\n");
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	free(hdr_token);
	json_free_serialized_string(body);
	json_value_free(root);
	return 0;
}

/* Deletes a movie by sending a DELETE request.
 * Prompts for movie ID and validates input.
 */
int handle_delete_movie(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("id=");
	char *id = helper_readline();
	if (!id || strlen(id) == 0) {
		printf("ERROR: id is required\n");
		return -1;
	}
	for (char *p = id; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: id must be a number\n");
			return -1;
		}
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_delete(ROUTE_MANAGE_MOVIE, id, sockfd, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			printf("SUCCESS: Film șters cu succes\n");
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	free(hdr_token);
	return 0;
}

/* Adds a new movie by sending a POST request with title,
 * year, description, and rating, validating each input.
 */
int handle_add_movie(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("title=");
	char *title = helper_readline();
	if (!title || strlen(title) == 0) {
		printf("ERROR: title is required\n");
		return -1;
	}

	printf("year=");
	char *temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: year is required\n");
		return -1;
	}
	for (char *p = temp; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: year must be a number\n");
			return -1;
		}
	}
	int year = atoi(temp);

	printf("description=");
	char *description = helper_readline();
	if (!description || strlen(description) == 0) {
		printf("ERROR: description is required\n");
		return -1;
	}

	printf("rating=");
	temp = helper_readline();
	if (!temp || strlen(temp) == 0) {
		printf("ERROR: rating is required\n");
		return -1;
	}
	{   int dot_count = 0;
	    for (char *p = temp; *p; ++p) {
	        if (*p == '.') {
	            if (++dot_count > 1) {
	                printf("ERROR: rating must be a valid number\n");
	                return -1;
	            }
	        } else if (!isdigit((unsigned char)*p)) {
	            printf("ERROR: rating must be a number\n");
	            return -1;
	        }
	    }
	}
	float rating = atof(temp);

	if (rating >= 10.0) {
		printf("ERROR: Rating must be between 0.0 and 10.0\n");
		return -1;
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "title", title);
	json_object_set_number(o, "year", year);
	json_object_set_string(o, "description", description);
	json_object_set_number(o, "rating", rating);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_MANAGE_MOVIE, body, PAYLOAD_APP_JSON,
							  sockfd, hdr_token);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			printf("SUCCESS: Film adăugat\n");
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	free(hdr_token);
	json_free_serialized_string(body);
	json_value_free(root);
	return 0;
}

/* Retrieves and prints details for a single movie.
 * Validates input and prompts for movie ID.
 */
int handle_get_movie(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	printf("id=");
	char *movie_id = helper_readline();
	if (!movie_id || strlen(movie_id) == 0) {
		printf("ERROR: id is required\n");
		return -1;
	}
	for (char *p = movie_id; *p; ++p) {
		if (!isdigit((unsigned char)*p)) {
			printf("ERROR: id must be a number\n");
			return -1;
		}
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_get(ROUTE_MANAGE_MOVIE, sockfd, hdr_token, movie_id);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
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

	free(hdr_token);
	return 0;
}

/* Retrieves and prints a list of all movies.
 * No additional input required beyond authorization.
 */
int handle_get_movies(char **token, int sockfd)
{
	if (!*token) {
		printf("ERROR: no access.\n");
		return -1;
	}

	char *hdr_token = malloc(HDR_COOKIE_SZ);
	if (hdr_token == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_token, HDR_COOKIE_SZ,
			 "Authorization: Bearer %s\r\n", *token);

	char *resp = request_get(ROUTE_MANAGE_MOVIE, sockfd, hdr_token, NULL);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_token);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
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

	free(hdr_token);
	return 0;
}

/* Exchanges a login cookie for a JWT access token.
 * No additional input beyond existing cookie.
 */
int handle_get_access(char **cookie, char **token, int sockfd)
{
	if (!*cookie) {
		printf("ERROR: login first.\n");
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
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
		if (status / 100 == 2) {
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

/* Logs out the current user by sending a GET request and clearing tokens.
 * No additional input required.
 */
int handle_logout(char **cookie, char **token, int sockfd)
{
	if (cookie == NULL || sockfd <= 0) {
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
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
		if (status / 100 == 2) {
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

/* Logs out the current admin by sending a GET request
 * and clearing the admin cookie.
 */
int handle_logout_admin(char **cookie, int sockfd)
{
	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
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
		if (status / 100 == 2) {
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

/* Prompts for admin and user credentials, validates non-empty, sends a login request,
 * and stores session cookie.
 */
int handle_login(char **cookie, int sockfd)
{
	if (*cookie) {
		printf("Already connected with an account\n");
		return 0;
	}

	printf("admin_username=");
	char *admin_username = helper_readline();
	if (!admin_username || strlen(admin_username) == 0) {
		printf("ERROR: admin_username is required\n");
		return -1;
	}

	printf("username=");
	char *username = helper_readline();
	if (!username || strlen(username) == 0) {
		printf("ERROR: username is required\n");
		return -1;
	}

	printf("password=");
	char *password = helper_readline();
	if (!password || strlen(password) == 0) {
		printf("ERROR: password is required\n");
		return -1;
	}

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "admin_username", admin_username);
	json_object_set_string(o, "username", username);
	json_object_set_string(o, "password", password);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_USER_LOGIN, body, PAYLOAD_APP_JSON,
							  sockfd, NULL);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			printf("SUCCESS: Autentificare reușită\n");
			*cookie = strdup(extract_cookie(resp));
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}

	return 0;
}

/* Deletes a user by username via DELETE request.
 * Prompts for username and validates non-empty input.
 */
int handle_delete_user(char **cookie, int sockfd)
{
	if (!*cookie) {
		printf("Error: login first.\n");
		return -1;
	}

	printf("username=");
	char *username = helper_readline();
	if (!username || strlen(username) == 0) {
		printf("ERROR: username is required\n");
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_cookie, HDR_COOKIE_SZ,
			 "Cookie: %s\r\n", *cookie);

	char *resp = request_delete(ROUTE_MANAGE_USER, username,
								sockfd, hdr_cookie);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
			printf("SUCCESS: Utilizator șters\n");
		} else {
			print_http_error(status, resp);
		}
		free(resp);
	}
	return 0;
}

/* Retrieves and prints a list of all users.
 * Requires an active session cookie, no extra input.
 */
int handle_get_users(char **cookie, int sockfd)
{
	if (!*cookie) {
		printf("Error: login first.\n");
		return -1;
	}

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
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
		if (status / 100 == 2) {
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

/* Adds a new user by sending a POST request with username and password.
 * Validates inputs for non-empty and no spaces in username.
 */
int handle_add_user(char **cookie, int sockfd)
{
	if (!*cookie) {
		printf("Error: login first.\n");
		return -1;
	}

	printf("username=");
	char *username = helper_readline();
	if (!username || strlen(username) == 0) {
		printf("ERROR: username is required\n");
		return -1;
	}

	printf("password=");
	char *password = helper_readline();
	if (!password || strlen(password) == 0) {
		printf("ERROR: password is required\n");
		return -1;
	}

	if (contains_space(username)) {
		printf("ERROR: username must not contain spaces\n");
		return -1;
	}

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "username", username);
	json_object_set_string(o, "password", password);
	char *body = json_serialize_to_string(root);

	char *hdr_cookie = malloc(HDR_COOKIE_SZ);
	if (hdr_cookie == NULL) {
		printf("ERROR: unable to allocate memory for cookie\n");
		exit(-1);
	}
	snprintf(hdr_cookie, HDR_COOKIE_SZ,
			 "Cookie: %s\r\n", *cookie);

	char *resp = request_post(ROUTE_MANAGE_USER, body, PAYLOAD_APP_JSON,
							  sockfd, hdr_cookie);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		free(hdr_cookie);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
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

/* Prompts for admin credentials, validates non-empty, sends a login request,
 * and stores admin session cookie.
 */
int handle_login_admin(char **cookie, int sockfd)
{
	if (*cookie) {
		printf("Already connected with an account\n");
		return 0;
	}

	printf("username=");
	char *username = helper_readline();
	if (!username || strlen(username) == 0) {
		printf("ERROR: username is required\n");
		return -1;
	}

	printf("password=");
	char *password = helper_readline();
	if (!password || strlen(password) == 0) {
		printf("ERROR: password is required\n");
		return -1;
	}

	JSON_Value *root = json_value_init_object();
	JSON_Object *o = json_value_get_object(root);
	json_object_set_string(o, "username", username);
	json_object_set_string(o, "password", password);
	char *body = json_serialize_to_string(root);

	char *resp = request_post(ROUTE_ADMIN_LOGIN, body, PAYLOAD_APP_JSON,
							  sockfd, NULL);
	if (!resp) {
		fprintf(stderr, "Error: no response\n");
		free(resp);
		json_free_serialized_string(body);
		json_value_free(root);
		return -1;
	} else {
		int status = get_status(resp);
		if (status / 100 == 2) {
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

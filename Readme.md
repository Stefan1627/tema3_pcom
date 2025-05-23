# Implementation Overview
# 324CC Stefan Calmac

- I implemented this homework in about 8 hours. I thought it was an ok homework in terms of description and it is of medium to easy difficulty.

---

## 1. Modular Structure

The project is split across four main layers:

1. **`helper.*`**  
   - Low-level utilities for I/O, string parsing, socket management, and JSON printing.  
   - Abstracts boilerplate like opening/closing connections, reading lines, stripping HTTP headers, extracting JSON fields, and formatting output.

2. **`requests.*`**  
   - Raw HTTP request builders and readers (GET, POST, PUT, DELETE).  
   - Constructs the request lines, headers, and bodies in fixed-size buffers, writes to the socket, and reads the response into a heap buffer.

3. **`commands.*`**  
   - High-level handlers for each user command (e.g. `handle_add_movie`, `handle_get_collections`).  
   - Each handler:
     - Prompts via `helper_readline()`
     - Builds JSON bodies using Parson APIs
     - Calls the appropriate `request_*` function
     - Checks HTTP status and prints success or error

4. **`main.c`**  
   - Dispatch loop that:
     - Reads a command string from stdin
     - Re-opens the TCP connection for each command
     - Invokes `commands_dispatch()` which string-compares the command and calls the right handler
     - Cleans up on exit

---

## 2. Connection & Session Management

- **One TCP connection per command**  
  To avoid stale or half-closed sockets, `commands_dispatch()` always calls `close()` then `setup_conn()`. This ensures a fresh `connect()` for every request.

- **Global state**  
  - `cookie` holds the session cookie returned by `login`/`login_admin`.  
  - `token` holds the JWT extracted by `get_access`.  
  These are passed by pointer into handlers so they can update or clear them as needed.

- **Stateless API calls**  
  Aside from the cookie and JWT, no other client-side state is kept. Handlers fully reconstruct each HTTP request.

---

## 3. HTTP Request Construction

- **Buffer-based formatting**  
  Each request is built into a fixed-size `char request[...]` buffer via `snprintf()`.  
  - GET/DELETE omit a JSON body  
  - POST/PUT include `Content-Type`, `Content-Length`, optional auth header, and the JSON payload

- **Error checking**  
  - Any `snprintf()` overflow or `write()`/`read()` error is checked immediately.  
  - On error, the functions print via `perror()` or `fprintf(stderr, …)` and return `NULL`.

- **Response handling**  
  - A single `read()` into an 8 KB heap buffer captures the entire response (headers + body).  
  - Caller uses `helper_strip_headers()` to isolate the JSON body when needed.

---

## 4. JSON Serialization & Parsing

- **Parson library**  
  - Handlers create JSON objects (`json_value_init_object()`, `json_object_set_*()`) for all POST/PUT bodies.  
  - Response bodies are parsed with `json_parse_string()`, then navigated via `json_value_get_object()`, `json_object_get_string()`, `json_object_get_number()`, and `json_object_get_array()`.

- **Utility functions in `helper.c`**  
  - `extract_id()` locates `"id":` in a raw HTTP response string and uses `strtol()` to parse it.  
  - `extract_token()` deserializes the JSON to pull out `"token"` and malloc’s a copy.  
  - Print functions (`print_movies()`, `print_users()`, etc.) iterate over Parson arrays and format each element.

---

## 5. Command Handlers & Control Flow

- **Uniform handler signature**  
  All handlers take pointers to `cookie` and/or `token` and the `sockfd`.  
  Return codes:  
  - ≥ 0 indicates success or specific status  
  - `< 0` indicates failure  
  - A special `EXIT` constant signals the main loop to terminate

- **Error propagation & rollback**  
  - If any HTTP request returns non-2xx, `print_http_error()` extracts the `"error"` field from the JSON body and prints it.  
  - In `handle_add_collection`, if adding one of the initial movies fails, it calls `handle_delete_collection()` to roll back the newly created collection.

- **Input validation**  
  - Handlers check for missing JWT (`if (!*token) { printf("ERROR: no access.\n"); ... }`)  
  - Handlers enforce domain rules (e.g. rating < 10.0, title/description non-empty, username contains no spaces)

---

## 6. Error Reporting

- **Network errors** via `perror("write")` or `perror("read")`.  
- **HTTP errors** via `print_http_error(int status, const char *resp)`, which:
  1. Finds the JSON body (`strchr(resp, '{')`)  
  2. Locates the `"error"` key  
  3. Extracts and prints the quoted message alongside the numeric status code

- **JSON parse errors** are reported with `fprintf(stderr, "Error: failed to parse JSON\n")`.

---

## 7. Key Design Tradeoffs

- **Simplicity over performance**  
  - Single `read()` per response; no streaming or chunk handling  
  - Reconnect per command simplifies socket lifecycle at the cost of extra TCP handshakes

- **Fixed buffers**  
  - `request[...]` and `buf[8192]` sizes chosen for typical payloads  
  - Handlers guard against overflow via `snprintf()` return checks

- **Minimal dependencies**  
  - Only standard POSIX socket APIs and the single-file Parson library  
  - No external HTTP or JSON frameworks

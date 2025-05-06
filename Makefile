CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -Wextra -O2

SRCS = client.c helper.c parson.c requests.c commands.c
OBJS = $(SRCS:.c=.o)
DEPS = client.h helper.h parson.h requests.h commands.h routes.h

all: client

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

client: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o client $(LDFLAGS)

check: client
	python3 checker/checker.py

clean:
	rm -f client $(OBJS)

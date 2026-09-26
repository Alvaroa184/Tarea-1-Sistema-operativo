CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11
EXEC = mishell

SRCS = mishell.c jobs.c pipes.c signals.c pmon.c

all: $(EXEC)

$(EXEC): $(SRCS) 
	$(CC) $(CFLAGS) -o $(EXEC) $(SRCS)

clean: 
	rm -f $(EXEC)
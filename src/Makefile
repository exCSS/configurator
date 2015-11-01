CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra
LDFLAGS = -lncurses -lpanel

build: main.o
	$(CC) $+ -o ../excss-configurator $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

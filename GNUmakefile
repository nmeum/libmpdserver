SOURCES = parser.c playback.c status.c list.c util.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = fns.h include/mpdserver.h

CFLAGS ?= -O0 -g -Werror
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200809L
CFLAGS += -Wpedantic -Wall -Wextra -Wconversion -Wmissing-prototypes \
	-Wpointer-arith -Wstrict-prototypes -Wshadow -Wcast-align

CPPFLAGS += -I./include -I./vendor/mpc

libmpdserver.a: mpc.o $(OBJECTS)
	$(AR) rcs $@ $^

mpc.o: mpc.c mpc.h
	$(CC) -c $< -o $@ $(CFLAGS) -w -ansi
%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

VPATH += vendor/mpc

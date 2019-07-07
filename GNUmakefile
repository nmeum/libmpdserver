SOURCES = parser.c playback.c status.c control.c queue.c \
	  database.c expr.c list.c util.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = fns.h include/mpdserver.h

CFLAGS ?= -O0 -g -Werror
CFLAGS += -std=c99 -D_POSIX_C_SOURCE=200809L
CFLAGS += -Wpedantic -Wall -Wextra -Wconversion -Wmissing-prototypes \
	-Wpointer-arith -Wstrict-prototypes -Wshadow -Wcast-align

CPPFLAGS += -I./include -I./vendor/mpc

libmpdserver.a: mpc.o $(OBJECTS)
	$(AR) rcs $@ $^
cmd2yaml: cmd2yaml.o libmpdserver.a
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

check: cmd2yaml
	cd tests && ./cmd2yaml.sh
valgrind: cmd2yaml
	cd tests && ./valgrind.sh
radamsa: cmd2yaml
	cd tests && ./radamsa.sh

format:
	clang-format -style=file -i cmd2yaml.c $(SOURCES) $(HEADERS)
	sed -i $(SOURCES) \
		-e 's/[ \t]*static mpc_parser_t/static mpc_parser_t/' \
		-e 's/static mpc_parser_t \*\(..*\)/static mpc_parser_t \*\n\1/'

mpc.o: mpc.c mpc.h
	$(CC) -c $< -o $@ $(CFLAGS) -w -ansi
%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

VPATH += vendor/mpc
.PHONY: check valgrind radamsa format

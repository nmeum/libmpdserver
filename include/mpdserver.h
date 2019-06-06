#ifndef MPDSERVER_H
#define MPDSERVER_H

#include <stddef.h>
#include <stdio.h>

typedef enum {
	MPD_VAL_INT,
	MPD_VAL_CMD,
} mpd_val_t;

typedef struct _mpd_argument_t mpd_argument_t;

typedef struct {
	char *name;
	size_t argc;
	mpd_argument_t **argv;
} mpd_command_t;

struct _mpd_argument_t {
	mpd_val_t type;
	union {
		int ival;
		mpd_command_t cmdval;
	} v;
};

mpd_command_t *mpd_parse(FILE *);
void mpd_free_command(mpd_command_t *);

#endif

#ifndef MPDSERVER_H
#define MPDSERVER_H

#include <stdio.h>

#include <sys/types.h>

typedef enum {
	MPD_VAL_INT,
	MPD_VAL_STR,
	MPD_VAL_FLOAT,
	MPD_VAL_RANGE,
	MPD_VAL_CMD,
} mpd_val_t;

typedef struct _mpd_argument_t mpd_argument_t;

typedef struct {
	char *name;
	size_t argc;
	mpd_argument_t **argv;
} mpd_command_t;

typedef struct {
	size_t start;
	ssize_t end; /* -1 if end is omitted */
} mpd_range_t;

struct _mpd_argument_t {
	mpd_val_t type;
	union {
		int ival;
		char *sval;
		float fval;
		mpd_range_t rval;
		mpd_command_t *cmdval;
	} v;
};

mpd_command_t *mpd_parse(FILE *);
void mpd_free_command(mpd_command_t *);

#endif

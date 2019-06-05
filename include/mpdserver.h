#ifndef MPDSERVER_H
#define MPDSERVER_H

#include <stddef.h>
#include <stdio.h>

typedef enum {
	MPD_CMD_LIST,
	MPD_CMD_LIST_OK,

	MPD_CMD_CONSUME,
	MPD_CMD_CROSSFADE,
	MPD_CMD_CURRENTSONG,
	MPD_CMD_STATUS,
} mpd_cmd_t;

typedef enum {
	MPD_VAL_INT,
	MPD_VAL_CMD,
} mpd_val_t;

typedef struct _mpd_argument_t mpd_argument_t;

typedef struct {
	mpd_cmd_t name;
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

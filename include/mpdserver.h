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

typedef union _mpd_argument_t mpd_argument_t;

typedef struct {
	mpd_cmd_t name;
	size_t argc;
	mpd_argument_t **argv;
} mpd_command_t;

union _mpd_argument_t {
	int ival;
	mpd_command_t cmdval;
};

mpd_command_t *mpd_parse(FILE *);
void mpd_free_command(mpd_command_t *);

#endif

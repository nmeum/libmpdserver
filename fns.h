#ifndef MPDSERVER_PARSER_H
#define MPDSERVER_PARSER_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mpc.h"
#include "mpdserver.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

#define mpdf_fold(I, N, ...)                                                   \
	static mpc_val_t *mpdf_##I(int n, mpc_val_t **xs)                      \
	{                                                                      \
		int i;                                                         \
		mpd_command_t *cmd;                                            \
                                                                               \
		i = 0;                                                         \
                                                                               \
		assert(n == (N + 1));                                          \
		assert(!strcmp(xs[0], #I));                                    \
                                                                               \
		cmd = mpd_new_command(#I, N);                                  \
		__VA_ARGS__                                                    \
                                                                               \
		for (i = 0; i < n; i++)                                        \
			free(xs[i]);                                           \
                                                                               \
		return cmd;                                                    \
	}

#define mpd_arg(TYPE, MEMBER, CAST)                                            \
	do {                                                                   \
		cmd->argv[i] = xmalloc(sizeof(mpd_argument_t));                \
		cmd->argv[i]->type = TYPE;                                     \
		cmd->argv[i]->v.MEMBER = *(CAST)xs[i + 1];                     \
		++i;                                                           \
	} while (0);

#define mpd_str_arg()                                                          \
	do {                                                                   \
		cmd->argv[i] = xmalloc(sizeof(mpd_argument_t));                \
		cmd->argv[i]->type = MPD_VAL_STR;                              \
		cmd->argv[i]->v.sval = xstrdup((char *)xs[i + 1]);             \
		++i;                                                           \
	} while (0);

mpc_parser_t *mpd_playback_cmds(void);
mpc_parser_t *mpd_status_cmds(void);
mpc_parser_t *mpd_list_cmds(void);

mpc_parser_t *mpd_argument(mpc_parser_t *);
mpc_parser_t *mpd_binary(void);
mpc_parser_t *mpd_string(void);
mpc_parser_t *mpd_command_primitive(void);
mpc_parser_t *mpd_cmd_noarg(char *);

void *xmalloc(size_t);
char *xstrdup(char *);
mpd_command_t *mpd_new_command(char *, size_t);

#endif

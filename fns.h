#ifndef MPDSERVER_PARSER_H
#define MPDSERVER_PARSER_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "mpc.h"
#include "mpdserver.h"

#define LENGTH(X) (sizeof(X) / sizeof(X[0]))

#define mpdf_fold(I, ...)                                                      \
	static mpc_val_t *mpdf_##I(int n, mpc_val_t **xs)                      \
	{                                                                      \
		int i, j;                                                      \
		mpd_command_t *cmd;                                            \
                                                                               \
		/* Remove optional mpc_maybe() arguments */                    \
		for (j = n - 1; j >= 0; j--)                                   \
			if (!xs[j])                                            \
				--n;                                           \
                                                                               \
		assert(n > 0);                                                 \
		cmd = mpd_new_command(xs[0], (size_t)n - 1);                   \
                                                                               \
		i = 1;                                                         \
		j = 0;                                                         \
		__VA_ARGS__                                                    \
                                                                               \
		return cmd;                                                    \
	}

#define mpdf_fold_args(I, N, ...)                                              \
	static mpc_val_t *mpdf_##I(int n, mpc_val_t **xs)                      \
	{                                                                      \
		int i, j;                                                      \
		mpd_command_t *cmd;                                            \
                                                                               \
		assert(n > 0);                                                 \
		cmd = mpd_new_command(xstrdup(N), (size_t)n);                  \
                                                                               \
		i = j = 0;                                                     \
		__VA_ARGS__                                                    \
                                                                               \
		return cmd;                                                    \
	}

#define mpd_arg(TYPE, MEMBER, CONV, FREE)                                      \
	do {                                                                   \
		cmd->argv[j] = xmalloc(sizeof(mpd_argument_t));                \
		cmd->argv[j]->type = TYPE;                                     \
		cmd->argv[j]->v.MEMBER = CONV xs[i];                           \
                                                                               \
		if (FREE)                                                      \
			free(xs[i]);                                           \
                                                                               \
		i++;                                                           \
		j++;                                                           \
	} while (0);

#define mpd_arg_many(ARG)                                                      \
	while (i < n) {                                                        \
		ARG                                                            \
	}

#define mpd_opt_arg(ARG)                                                       \
	if (n > i) {                                                           \
		ARG                                                            \
	}

#define MPD_ARG_INT mpd_arg(MPD_VAL_INT, ival, *(int *), 1)
#define MPD_ARG_UINT mpd_arg(MPD_VAL_UINT, uval, *(unsigned int *), 1)
#define MPD_ARG_FLOAT mpd_arg(MPD_VAL_FLOAT, fval, *(float *), 1)
#define MPD_ARG_BOOL mpd_arg(MPD_VAL_BOOL, bval, *(int *), 1)
#define MPD_ARG_RANGE mpd_arg(MPD_VAL_RANGE, rval, *(mpd_range_t *), 1)
#define MPD_ARG_STRING mpd_arg(MPD_VAL_STR, sval, (char *), 0)
#define MPD_ARG_EXPR mpd_arg(MPD_VAL_EXPR_STR, sval, (char *), 0)

typedef struct {
	char **ptr;
	size_t len;
} mpd_string_array_t;

/* clang-format off */
#define MPD_STRING_ARY(ARY) {ARY, LENGTH(ARY)}
/* clang-format on */

mpc_parser_t *mpd_playback_cmds(void);
mpc_parser_t *mpd_status_cmds(void);
mpc_parser_t *mpd_list_cmds(void);
mpc_parser_t *mpd_control_cmds(void);
mpc_parser_t *mpd_queue_cmds(void);
mpc_parser_t *mpd_database_cmds(void);
mpc_parser_t *mpd_connection_cmds(void);

mpc_parser_t *mpd_argument(mpc_parser_t *);
mpc_parser_t *mpd_int(void);
mpc_parser_t *mpd_uint(void);
mpc_parser_t *mpd_binary(void);
mpc_parser_t *mpd_float_digits(void);
mpc_parser_t *mpd_float(void);
mpc_parser_t *mpd_range(void);
mpc_parser_t *mpd_range_with_single(void);
mpc_parser_t *mpd_string(void);
mpc_parser_t *mpd_string_case(void);
mpc_parser_t *mpd_uri(void);
mpc_parser_t *mpd_command_primitive(void);
mpc_parser_t *mpd_cmd_noarg(char *);
mpc_parser_t *mpd_subcommand(char *, char *);
mpc_parser_t *mpd_whitespace(void);
mpc_parser_t *mpd_tag_name(void);

mpc_val_t *mpdf_change_name(mpc_val_t *, void *);
mpc_val_t *mpdf_command_noarg(mpc_val_t *);
mpc_val_t *mpdf_lowercase(mpc_val_t *);
mpc_val_t *mpdf_unescape(mpc_val_t *);

void *xmalloc(size_t);
char *xstrdup(char *);
void *xrealloc(void *, size_t);
char *lowercase(char *);
mpd_range_t *mpd_new_range(size_t, ssize_t);
mpd_command_t *mpd_new_command(char *, size_t);
void mpd_free_expression(void *);
mpd_expression_t *mpd_expression(char *);
int mpd_check_array(mpc_val_t **, void *);

#endif

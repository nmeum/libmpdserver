#include "fns.h"
#include "mpc.h"

/* This file provides some custom fold functions for dealing with
 * MPD commands taking variadic arguments, if these are needed
 * elsewhere at some point the concept needs to be generalized. */

static mpc_val_t *
mpdf_argvfold(int n, mpc_val_t **xs)
{
	int i;
	mpd_command_t *cmd;

	cmd = mpd_new_command(NULL, (size_t)n);
	for (i = 0; i < n; i++) {
		cmd->argv[i] = xmalloc(sizeof(mpd_argument_t));
		cmd->argv[i]->type = MPD_VAL_STR;
		cmd->argv[i]->v.sval = (char *)xs[i];
	}

	return cmd;
}

static mpc_val_t *
mpdf_cmdfold(int n, mpc_val_t **xs)
{
	size_t i, j, oldargc;
	mpd_command_t *cmd;

	assert(n >= 3);
	cmd = (mpd_command_t *)xs[n - 1];
	cmd->name = (char *)xs[0];

	n -= 2;
	oldargc = cmd->argc;
	cmd->argc += (size_t)n;

	cmd->argv = xrealloc(cmd->argv, sizeof(mpd_argument_t *) * cmd->argc);
	memmove(&cmd->argv[n], cmd->argv, sizeof(mpd_argument_t *) * oldargc);

	for (i = 0, j = 1; i < (size_t)n; i++, j++) {
		cmd->argv[i] = xmalloc(sizeof(mpd_argument_t));
		cmd->argv[i]->type = MPD_VAL_STR;
		cmd->argv[i]->v.sval = xs[j];
	}

	return cmd;
}

static mpc_parser_t *
mpd_ping(void)
{
	return mpd_cmd_noarg("ping");
}

mpdf_fold(tagtypes_clear, MPD_ARG_STRING)

static mpc_parser_t *
mpd_tagtypes_clear(void)
{
	return mpc_and(2, mpdf_tagtypes_clear, mpc_string("tagtypes"),
	               mpd_argument(mpc_string("clear")), free);
}

mpdf_fold(tagtypes_all, MPD_ARG_STRING)

static mpc_parser_t *
mpd_tagtypes_all(void)
{
	return mpc_and(2, mpdf_tagtypes_all, mpc_string("tagtypes"),
	               mpd_argument(mpc_string("all")), free);
}

static mpc_parser_t *
mpd_tagtypes_disable(void)
{
	mpc_parser_t *args;

	args = mpc_many1(mpdf_argvfold, mpd_argument(mpd_string()));
	return mpc_and(3, mpdf_cmdfold, mpc_string("tagtypes"),
	               mpd_argument(mpc_string("disable")), args, free, free);
}

static mpc_parser_t *
mpd_tagtypes_enable(void)
{
	mpc_parser_t *args;

	args = mpc_many1(mpdf_argvfold, mpd_argument(mpd_string()));
	return mpc_and(3, mpdf_cmdfold, mpc_string("tagtypes"),
	               mpd_argument(mpc_string("enable")), args, free, free);
}

static mpc_parser_t *
mpd_tagtypes(void)
{
	return mpd_cmd_noarg("tagtypes");
}

mpc_parser_t *
mpd_connection_cmds(void)
{
	return mpc_or(6, mpd_ping(), mpd_tagtypes_all(), mpd_tagtypes_clear(),
	              mpd_tagtypes_disable(), mpd_tagtypes_enable(),
	              mpd_tagtypes());
}

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fns.h"
#include "mpc.h"
#include "mpdserver.h"

static mpc_parser_t *
mpd_command(void)
{
	return mpc_or(2, mpd_list_cmds(), mpd_command_primitive());
}

static int
mpd_check_quote(mpc_val_t **val)
{
	char *ptr;

	ptr = (char *)*val;
	return *ptr != '"';
}

static mpc_parser_t *
mpd_whitespace(void)
{
	return mpc_expect(mpc_oneof(" \t"), "whitespace");
}

static mpc_val_t *
mpdf_command_noarg(mpc_val_t *val)
{
	mpd_command_t *cmd;
	char *str;

	str = (char *)val;
	cmd = mpd_new_command(str, 0);

	free(val);
	return cmd;
}

mpc_parser_t *
mpd_cmd_noarg(char *cmdstr)
{
	return mpc_apply(mpc_string(cmdstr), mpdf_command_noarg);
}

mpc_parser_t *
mpd_argument(mpc_parser_t *a)
{
	mpc_parser_t *sep;

	sep = mpc_expect(mpd_whitespace(), "argument");
	return mpc_and(2, mpcf_snd_free, sep, a, free);
}

mpc_parser_t *
mpd_binary(void)
{
	return mpc_expect(mpc_apply(mpc_oneof("01"), mpcf_int), "binary");
}

mpc_parser_t *
mpd_time(void)
{
	mpc_parser_t *pre, *sep, *fra, *par;

	pre = mpc_maybe_lift(mpc_digits(), mpcf_ctor_str);
	sep = mpc_char('.');

	fra = mpc_and(3, mpcf_strfold, pre, sep, mpc_digits(), free, free);
	par = mpc_or(2, fra, mpc_digits(), free);

	return mpc_expect(mpc_apply(par, mpcf_float), "time");
}

mpc_parser_t *
mpd_string(void)
{
	mpc_parser_t *str, *strcheck;

	str = mpc_many(mpcf_strfold, mpc_noneof(" \t\n"));
	strcheck = mpc_check(str, mpd_check_quote, "missing closing '\"'");

	return mpc_or(2, mpc_string_lit(), strcheck);
}

mpc_parser_t *
mpd_command_primitive(void)
{
	mpc_parser_t *cmd;

	/* TODO: mpc_or all parsers */
	cmd = mpc_or(3, mpd_playback_cmds(), mpd_status_cmds(),
	             mpd_control_cmds());

	return mpc_and(2, mpcf_fst, cmd, mpc_newline(), free);
}

mpd_command_t *
mpd_parse(FILE *stream)
{
	mpc_result_t r;

	if (mpc_parse_pipe("", stream, mpd_command(), &r)) {
		return (mpd_command_t *)r.output;
	} else {
		return NULL;
	}
}

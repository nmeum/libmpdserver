#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

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

mpc_parser_t *
mpd_whitespace(void)
{
	return mpc_expect(mpc_oneof(" \t"), "whitespace");
}

mpc_val_t *
mpdf_unescape(mpc_val_t *val)
{
	unsigned short esc;
	size_t i, oldlen, newlen;
	char *oldstr, *newstr;

	oldstr = val;
	oldlen = strlen(oldstr);

	newlen = 0;
	newstr = xmalloc(sizeof(char) * oldlen);

	for (esc = 0, i = 0; i < oldlen; i++) {
		if (oldstr[i] == '\\' && !esc) {
			esc = 1;
			continue;
		}

		newstr[newlen++] = oldstr[i];
		esc = 0;
	}

	free(val);
	newstr = xrealloc(newstr, newlen + 1);
	newstr[newlen] = '\0';
	return newstr;
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

static mpc_val_t *
mpdf_fold_range(int n, mpc_val_t **xs)
{
	int i;
	int start, end;

	assert(n == 3);
	assert(*(char *)xs[1] == ':');

	start = atoi((char *)xs[0]);
	end = (xs[2]) ? atoi((char *)xs[2]) : -1;

	for (i = 0; i < n; i++)
		free(xs[i]);

	/* TODO: start shouldn't be negative, but might be due to an
	 * integer overflow. Unfortunately, we can't signal an error
	 * from a fold function and thus can't handle this properly. */
	/* assert(start >= 0); */
	return mpd_new_range((size_t)start, (ssize_t)end);
}

mpc_val_t *
mpdf_range(mpc_val_t *val)
{
	int pos;

	pos = *(int *)val;
	/* TODO: pos shouldn't be negative, but might be due to an
	 * integer overflow. Unfortunately, we can't signal an error
	 * from a apply function and thus can't handle this properly. */
	/* assert(pos >= 0); */
	free(val);

	return mpd_new_range((size_t)pos, (size_t)pos);
}

mpc_parser_t *
mpd_cmd_noarg(char *cmdstr)
{
	return mpc_apply(mpc_string(cmdstr), mpdf_command_noarg);
}

mpc_parser_t *
mpd_argument(mpc_parser_t *a)
{
	mpc_parser_t *sep, *spaces;

	spaces = mpc_many1(mpcf_fst_free, mpd_whitespace());
	sep = mpc_expect(spaces, "argument");

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
mpd_range(void)
{
	return mpc_and(3, mpdf_fold_range, mpc_digits(), mpc_char(':'),
	               mpc_maybe(mpc_digits()), free, free);
}

mpc_parser_t *
mpd_string(void)
{
	mpc_parser_t *str, *strcheck;

	str = mpc_many(mpcf_strfold, mpc_noneof(" \t\n"));
	strcheck = mpc_check(str, mpd_check_quote, "missing closing '\"'");

	return mpc_or(2, mpc_apply(mpc_string_lit(), mpdf_unescape), strcheck);
}

mpc_parser_t *
mpd_uri(void)
{
	/* Unfortunately, a URI argument is either a URI or a file name.
	 * While a URI is properly defined in RFC 3986 a file name can
	 * be pretty much anything though mpd itself doesn't seem to
	 * allow `.` and `..` as path elements in file names.
	 * Nonetheless, it doesn't make sense to validate URIs properly
	 * since every invalid URI is likely a valid file name. */
	return mpd_string();
}

mpc_parser_t *
mpd_command_primitive(void)
{
	mpc_parser_t *cmd;

	/* TODO: mpc_or all parsers */
	cmd = mpc_or(4, mpd_playback_cmds(), mpd_status_cmds(),
	             mpd_control_cmds(), mpd_queue_cmds());

	return mpc_and(2, mpcf_fst, cmd, mpc_newline(), mpd_free_command);
}

mpd_command_t *
mpd_parse(FILE *stream)
{
	size_t i;
	mpd_expression_t *expr;
	mpd_argument_t *c;
	mpd_command_t *cmd;
	mpc_parser_t *par;
	mpc_result_t r;

	cmd = NULL;
	par = mpd_command();

	if (mpc_parse_pipe("", stream, par, &r)) {
		cmd = (mpd_command_t *)r.output;
	} else {
		mpc_err_delete(r.error);
		goto ret;
	}

	/* TODO: There is now easy way to unescape the input string
	 * before processing it further. Woraround: call mpc_parse twice. */
	for (i = 0; i < cmd->argc; i++) {
		c = cmd->argv[i];
		if (c->type != MPD_VAL_EXPR_STR)
			continue;

		if (!(expr = mpd_expression(c->v.sval))) {
			mpd_free_command(cmd);
			goto ret;
		}

		free(c->v.sval);
		c->v.eval = expr;
		c->type = MPD_VAL_EXPR;
	}

ret:
	mpc_cleanup(1, par);
	return cmd;
}

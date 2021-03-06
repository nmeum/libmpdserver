#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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

int
mpd_check_array(mpc_val_t **val, void *ptr)
{
	size_t i;
	int inset;
	char *str;
	mpd_string_array_t *ary;

	str = *(char **)val;
	ary = (mpd_string_array_t *)ptr;

	inset = 0;
	for (i = 0; i < ary->len; i++) {
		if (!strcasecmp(str, ary->ptr[i]))
			inset = 1;
	}

	return inset;
}

mpc_parser_t *
mpd_whitespace(void)
{
	return mpc_expect(mpc_oneof(" \t"), "whitespace");
}

mpc_val_t *
mpdf_lowercase(mpc_val_t *val)
{
	return lowercase((char *)val);
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

	return cmd;
}

static mpc_val_t *
mpdf_fold_range(int n, mpc_val_t **xs)
{
	int i;
	size_t start;
	ssize_t end;

	assert(n == 3);
	assert(*(char *)xs[1] == ':');

	/* TODO: We can't signal an error error condition from an apply
	 * function and strtoull(3) might potentially overflow. */
	start = (size_t)strtoull((char *)xs[0], NULL, 10);
	end = (xs[2]) ? (ssize_t)strtoull((char *)xs[2], NULL, 10) : -1;

	for (i = 0; i < n; i++)
		free(xs[i]);

	return mpd_new_range(start, end);
}

static mpc_val_t *
mpdf_range(mpc_val_t *val)
{
	unsigned int pos;

	pos = *(unsigned int *)val;
	free(val);

	return mpd_new_range((size_t)pos, (ssize_t)pos);
}

static mpc_val_t *
mpdf_uint(mpc_val_t *val)
{
	unsigned int *uval;

	uval = xmalloc(sizeof(*uval));
	/* TODO: We can't signal an error error condition from an apply
	 * function and strtoul(3) might potentially overflow. */
	*uval = (unsigned int)strtoul((char *)val, NULL, 10);

	free(val);
	return uval;
}

mpc_parser_t *
mpd_cmd_noarg(char *cmdstr)
{
	return mpc_apply(mpc_string(cmdstr), mpdf_command_noarg);
}

mpc_parser_t *
mpd_argument(mpc_parser_t *a)
{
	mpc_parser_t *sep, *spaces, *quoted;

	spaces = mpc_many1(mpcf_freefold, mpd_whitespace());
	sep = mpc_expect(spaces, "argument");

	/* All MPD arguments can be enclosed in quotes. However, some
	 * argument parsers, e.g. mpd_string, handle quotes explicitly,
	 * thus they may have already been parsed at this point and are
	 * only optional. */

	/* TODO: unescape all arguments before doing further parsing */

	quoted = mpc_between(mpc_copy(a), free, "\"", "\"");
	return mpc_and(2, mpcf_snd_free, sep, mpc_or(2, a, quoted), free);
}

mpc_parser_t *
mpd_int(void)
{
	mpc_parser_t *neg, *val;

	neg = mpc_and(2, mpcf_strfold, mpc_char('-'), mpc_digits(), free);
	val = mpc_or(2, neg, mpc_digits());

	return mpc_expect(mpc_apply(val, mpcf_int), "signed integer");
}

mpc_parser_t *
mpd_uint(void)
{
	return mpc_expect(mpc_apply(mpc_digits(), mpdf_uint),
	                  "unsigned integer");
}

mpc_parser_t *
mpd_binary(void)
{
	return mpc_expect(mpc_apply(mpc_oneof("01"), mpdf_uint), "binary");
}

mpc_parser_t *
mpd_float_digits(void)
{
	mpc_parser_t *pre, *sep, *fra;

	pre = mpc_maybe_lift(mpc_digits(), mpcf_ctor_str);
	sep = mpc_char('.');

	fra = mpc_and(3, mpcf_strfold, pre, sep, mpc_digits(), free, free);
	return mpc_or(2, fra, mpc_digits(), free);
}

mpc_parser_t *
mpd_float(void)
{
	return mpc_expect(mpc_apply(mpd_float_digits(), mpcf_float), "float");
}

mpc_parser_t *
mpd_range(void)
{
	return mpc_and(3, mpdf_fold_range, mpc_digits(), mpc_char(':'),
	               mpc_maybe(mpc_digits()), free, free);
}

mpc_parser_t *
mpd_range_with_single(void)
{
	mpc_parser_t *single;

	single = mpc_apply(mpd_uint(), mpdf_range);
	return mpc_or(2, mpd_range(), single);
}

mpc_parser_t *
mpd_string(void)
{
	mpc_parser_t *str, *strcheck;

	str = mpc_many(mpcf_strfold, mpc_noneof(" \t\n"));
	strcheck =
	    mpc_check(str, free, mpd_check_quote, "missing closing '\"'");

	return mpc_or(2, mpc_apply(mpc_string_lit(), mpdf_unescape), strcheck);
}

mpc_parser_t *
mpd_string_case(void)
{
	return mpc_apply(mpd_string(), mpdf_lowercase);
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
	cmd = mpc_or(6, mpd_playback_cmds(), mpd_status_cmds(),
	             mpd_queue_cmds(), mpd_control_cmds(), mpd_database_cmds(),
	             mpd_connection_cmds());

	return mpc_and(2, mpcf_fst_free, cmd, mpc_newline(), mpd_free_command);
}

mpd_command_t *
mpd_parse(char *input)
{
	size_t i;
	mpd_expression_t *expr;
	mpd_argument_t *c;
	mpd_command_t *cmd;
	mpc_parser_t *par;
	mpc_result_t r;

	cmd = NULL;
	par = mpd_command();

	if (mpc_parse("", input, par, &r)) {
		cmd = (mpd_command_t *)r.output;
	} else {
#ifdef MPC_PARSER_DEBUG
		mpc_err_print(r.error);
#endif
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
			cmd = NULL;
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

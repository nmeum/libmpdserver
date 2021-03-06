#include <ctype.h>
#include <err.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "fns.h"

void *
xmalloc(size_t size)
{
	void *r;

	if (!(r = malloc(size)))
		err(EXIT_FAILURE, "malloc failed");

	return r;
}

char *
xstrdup(char *s)
{
	char *r;

	if (!(r = strdup(s)))
		err(EXIT_FAILURE, "strdup failed");

	return r;
}

void *
xrealloc(void *ptr, size_t size)
{
	void *r;

	if (!(r = realloc(ptr, size)))
		err(EXIT_FAILURE, "realloc failed");

	return r;
}

char *
lowercase(char *str)
{
	size_t i;

	for (i = 0; i < strlen(str); i++)
		str[i] = (char)tolower(str[i]);

	return str;
}

mpd_range_t *
mpd_new_range(size_t start, ssize_t end)
{
	mpd_range_t *range;

	range = xmalloc(sizeof(*range));
	range->start = start;
	range->end = end;

	return range;
}

mpd_command_t *
mpd_new_command(char *name, size_t argc)
{
	mpd_command_t *cmd;

	cmd = xmalloc(sizeof(*cmd));
	cmd->name = name; /* might be NULL */
	cmd->argc = argc;

	if (argc > 0)
		cmd->argv = xmalloc(sizeof(mpd_argument_t *) * argc);
	else
		cmd->argv = NULL;

	return cmd;
}

static void
mpd_free_argument(mpd_argument_t *arg)
{
	if (arg->type == MPD_VAL_STR || arg->type == MPD_VAL_EXPR_STR)
		free(arg->v.sval);
	else if (arg->type == MPD_VAL_CMD)
		mpd_free_command(arg->v.cmdval);
	else if (arg->type == MPD_VAL_EXPR)
		mpd_free_expression(arg->v.eval);
	free(arg);
}

void
mpd_free_command(mpd_command_t *cmd)
{
	size_t i;

	if (cmd->name)
		free(cmd->name);
	for (i = 0; i < cmd->argc; i++)
		mpd_free_argument(cmd->argv[i]);
	if (cmd->argv)
		free(cmd->argv);
	free(cmd);
}

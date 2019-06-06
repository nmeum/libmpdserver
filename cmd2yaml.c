#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "mpdserver.h"

static void format_cmd(int, mpd_command_t *);

static void
format_arg(int ident, mpd_argument_t *arg)
{
	switch (arg->type) {
		case MPD_VAL_INT:
			printf("%d\n", arg->v.ival);
			break;
		case MPD_VAL_CMD:
			printf("\n");
			format_cmd(ident + 2, &arg->v.cmdval);
			break;
		default:
			errx(EXIT_FAILURE, "unsupported type: %u\n", arg->type);
			break;
	}
}

static void
format_cmd(int ident, mpd_command_t *cmd)
{
	size_t i;
	mpd_argument_t *arg;

	printf("%*sname: %s\n", ident, "", cmd->name);
	printf("%*sargc: %zu\n", ident, "", cmd->argc);
	printf("%*sargv:\n", ident, "");

	ident += 2;
	for (i = 0; i < cmd->argc; i++) {
		arg = cmd->argv[i];

		printf("%*s- type: %u\n", ident, "", arg->type);
		ident += 2;

		printf("%*svalue: ", ident, "");
		format_arg(ident, arg);

		ident -= 2;
	}
}

int
main(void)
{
	mpd_command_t *cmd;

	if (!(cmd = mpd_parse(stdin)))
		return EXIT_FAILURE;

	format_cmd(0, cmd);
	mpd_free_command(cmd);

	return EXIT_SUCCESS;
}

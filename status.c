#include <stddef.h>
#include <string.h>

#include "fns.h"
#include "mpc.h"

static char *mpd_subsystems[] = {
    "database",  "update",  "stored_playlist", "playlist",
    "player",    "mixer",   "output",          "options",
    "partition", "sticker", "subscription",    "message",
};

static int
mpd_check_subsys(mpc_val_t **val)
{
	size_t i;
	char *str;
	int inset;

	str = *(char **)val;
	for (inset = 0, i = 0; i < LENGTH(mpd_subsystems); i++) {
		if (!strcmp(str, mpd_subsystems[i]))
			inset = 1;
	}

	return inset;
}

static mpc_parser_t *
mpd_currentsong(void)
{
	return mpd_cmd_noarg("currentsong");
}

static mpc_parser_t *
mpd_status(void)
{
	return mpd_cmd_noarg("status");
}

mpdf_fold(idle, 1, mpd_str_arg())

static mpc_parser_t *mpd_idle(void)
{
	mpc_parser_t *subsys;

	subsys = mpc_check(mpd_string(), mpd_check_subsys, "invalid subsystem");
	return mpc_and(2, mpdf_idle, mpc_string("idle"), mpd_argument(subsys),
	               free);
}

mpc_parser_t *
mpd_status_cmds(void)
{
	return mpc_or(3, mpd_currentsong(), mpd_status(), mpd_idle());
}

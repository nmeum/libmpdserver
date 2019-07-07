#include <stddef.h>
#include <string.h>

#include "fns.h"
#include "mpc.h"

static char *mpd_subsystems[] = {
    "database",  "update",  "stored_playlist", "playlist",
    "player",    "mixer",   "output",          "options",
    "partition", "sticker", "subscription",    "message",
};

static mpc_parser_t *
mpd_clearerror(void)
{
	return mpd_cmd_noarg("clearerror");
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

static mpc_parser_t *
mpd_stats(void)
{
	return mpd_cmd_noarg("stats");
}

mpdf_fold(idle, mpd_opt_arg(MPD_ARG_STRING))

static mpc_parser_t *
mpd_idle(void)
{
	mpc_parser_t *subsys;
	static mpd_string_array_t ary = MPD_STRING_ARY(mpd_subsystems);

	subsys = mpc_check_with(mpd_string_case(), free, mpd_check_array, &ary,
	                        "invalid subsystem");
	return mpc_and(2, mpdf_idle, mpc_string("idle"),
	               mpc_maybe(mpd_argument(subsys)), free);
}

mpc_parser_t *
mpd_status_cmds(void)
{
	return mpc_or(5, mpd_clearerror(), mpd_currentsong(), mpd_idle(),
	              mpd_status(), mpd_stats());
}

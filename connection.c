#include "fns.h"
#include "mpc.h"

/* TODO: Handling of subcommands must be improvement, the subcommand
 * actually constitutes an mpd_argument(). As such, commands like
 * `tagtypes "disable" foo` are incorrectly considered invalid by
 * libmpdserver. */

static mpc_parser_t *
mpd_ping(void)
{
	return mpd_cmd_noarg("ping");
}

mpdf_fold(tagtypes_disable, MPD_ARG_LIST)

static mpc_parser_t *
mpd_tagtypes_disable(void)
{
	return mpc_and(2, mpdf_tagtypes_disable, mpc_string("tagtypes disable"),
	               mpd_list(), free);
}

mpc_parser_t *
mpd_connection_cmds(void)
{
	return mpc_or(2, mpd_ping(), mpd_tagtypes_disable());
}

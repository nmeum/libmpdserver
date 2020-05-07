#include "fns.h"
#include "mpc.h"

static mpc_parser_t *
mpd_ping(void)
{
	return mpd_cmd_noarg("ping");
}

static mpc_parser_t *
mpd_tagtypes_all(void)
{
	return mpc_apply(mpd_subcommand("tagtypes", "all"), mpdf_command_noarg);
}

mpdf_fold(tagtypes_disable, MPD_ARG_LIST)

static mpc_parser_t *
mpd_tagtypes_disable(void)
{
	return mpc_and(2, mpdf_tagtypes_disable,
	               mpd_subcommand("tagtypes", "disable"), mpd_list(), free);
}

mpdf_fold(tagtypes_enable, MPD_ARG_LIST)

static mpc_parser_t *
mpd_tagtypes_enable(void)
{
	return mpc_and(2, mpdf_tagtypes_enable,
	               mpd_subcommand("tagtypes", "enable"), mpd_list(), free);
}

static mpc_parser_t *
mpd_tagtypes(void)
{
	return mpd_cmd_noarg("tagtypes");
}

mpc_parser_t *
mpd_connection_cmds(void)
{
	return mpc_or(5, mpd_ping(), mpd_tagtypes_all(), mpd_tagtypes_disable(),
	              mpd_tagtypes_enable(), mpd_tagtypes());
}

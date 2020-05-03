#include "fns.h"
#include "mpc.h"

static mpc_parser_t *
mpd_ping(void)
{
	return mpd_cmd_noarg("ping");
}

mpc_parser_t *
mpd_connection_cmds(void)
{
	return mpd_ping();
}

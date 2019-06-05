#include "fns.h"
#include "mpc.h"

static mpc_parser_t *
mpd_currentsong(void)
{
	return mpd_cmd_noarg("currentsong", MPD_CMD_CURRENTSONG);
}

static mpc_parser_t *
mpd_status(void)
{
	return mpd_cmd_noarg("status", MPD_CMD_STATUS);
}

mpc_parser_t *
mpd_status_cmds(void)
{
	return mpc_or(2, mpd_currentsong(), mpd_status());
}

#include "fns.h"
#include "mpc.h"

mpdf_fold(count, MPD_ARG_EXPR)

static mpc_parser_t *
mpd_count(void)
{
	return mpc_and(2, mpdf_count, mpc_string("count"),
	               mpd_argument(mpd_string()), free);
}

mpc_parser_t *
mpd_database_cmds(void)
{
	return mpd_count();
}

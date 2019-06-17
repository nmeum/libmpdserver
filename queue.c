#include "fns.h"
#include "mpc.h"

mpdf_fold(add, MPD_ARG_STRING)

static mpc_parser_t *
mpd_add(void)
{
	return mpc_and(2, mpdf_add, mpc_string("add"), mpd_argument(mpd_uri()),
	               free);
}

mpdf_fold(delete, MPD_ARG_RANGE)

static mpc_parser_t *
mpd_delete(void)
{
	return mpc_and(2, mpdf_delete, mpc_string("delete"),
	               mpd_argument(mpd_range()), free);
}

mpc_parser_t *
mpd_queue_cmds(void)
{
	return mpc_or(2, mpd_add(), mpd_delete());
}

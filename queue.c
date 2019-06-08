#include "fns.h"
#include "mpc.h"

mpdf_fold(delete, 1, MPD_ARG_RANGE)

static mpc_parser_t *
mpd_delete(void)
{
	mpc_parser_t *pos;

	pos = mpc_or(2, mpd_range(), mpc_apply(mpc_int(), mpdf_range));
	return mpc_and(2, mpdf_delete, mpc_string("delete"), mpd_argument(pos),
	               free);
}

mpc_parser_t *
mpd_queue_cmds(void)
{
	return mpd_delete();
}

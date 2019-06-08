#include "fns.h"
#include "mpc.h"

mpdf_fold(seek, 2, MPD_ARG_INT MPD_ARG_FLOAT)

static mpc_parser_t *mpd_seek(void)
{
	return mpc_and(3, mpdf_seek, mpc_string("seek"),
	               mpd_argument(mpc_int()), mpd_argument(mpd_time()), free,
	               free);
}

mpc_parser_t *
mpd_control_cmds(void)
{
	return mpd_seek();
}

#include "fns.h"
#include "mpc.h"

mpdf_fold(seek, MPD_ARG_UINT MPD_ARG_FLOAT)

static mpc_parser_t *
mpd_seek(void)
{
	return mpc_and(3, mpdf_seek, mpc_string("seek"),
	               mpd_argument(mpd_uint()), mpd_argument(mpd_float()),
	               free, free);
}

mpc_parser_t *
mpd_control_cmds(void)
{
	return mpd_seek();
}

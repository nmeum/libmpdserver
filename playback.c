#include "fns.h"
#include "mpc.h"

mpdf_fold(consume, 1, mpd_arg(MPD_VAL_INT, ival, int *))

static mpc_parser_t *mpd_consume(void)
{
	return mpc_and(2, mpdf_consume, mpd_cmdstr("consume"),
	               mpd_argument(mpd_binary()), free);
}

mpdf_fold(crossfade, 1, mpd_arg(MPD_VAL_INT, ival, int *))

static mpc_parser_t *mpd_crossfade(void)
{
	return mpc_and(2, mpdf_crossfade, mpd_cmdstr("crossfade"),
	               mpd_argument(mpc_int()), free);
}

mpc_parser_t *
mpd_playback_cmds(void)
{
	return mpc_or(2, mpd_consume(), mpd_crossfade());
}

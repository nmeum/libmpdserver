#include "fns.h"
#include "mpc.h"

mpdf_fold(consume, MPD_ARG_INT)

static mpc_parser_t *
mpd_consume(void)
{
	return mpc_and(2, mpdf_consume, mpc_string("consume"),
	               mpd_argument(mpd_binary()), free);
}

mpdf_fold(crossfade, MPD_ARG_INT)

static mpc_parser_t *
mpd_crossfade(void)
{
	return mpc_and(2, mpdf_crossfade, mpc_string("crossfade"),
	               mpd_argument(mpc_int()), free);
}

mpc_parser_t *
mpd_playback_cmds(void)
{
	return mpc_or(2, mpd_consume(), mpd_crossfade());
}

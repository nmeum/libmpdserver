#include "fns.h"
#include "mpc.h"

static int
mpd_check_volume(mpc_val_t **val)
{
	unsigned int *ptr;

	ptr = *(unsigned **)val;
	return *ptr <= 100;
}

mpdf_fold(consume, MPD_ARG_UINT)

static mpc_parser_t *
mpd_consume(void)
{
	return mpc_and(2, mpdf_consume, mpc_string("consume"),
	               mpd_argument(mpd_binary()), free);
}

mpdf_fold(crossfade, MPD_ARG_UINT)

static mpc_parser_t *
mpd_crossfade(void)
{
	return mpc_and(2, mpdf_crossfade, mpc_string("crossfade"),
	               mpd_argument(mpd_uint()), free);
}

mpdf_fold(setvol, MPD_ARG_UINT)

static mpc_parser_t *
mpd_setvol(void)
{
	mpc_parser_t *vol;

	vol = mpc_check(mpd_uint(), free, mpd_check_volume, "invalid volume");
	return mpc_and(2, mpdf_setvol, mpc_string("setvol"), mpd_argument(vol),
	               free);
}

mpc_parser_t *
mpd_playback_cmds(void)
{
	return mpc_or(3, mpd_consume(), mpd_crossfade(), mpd_setvol());
}

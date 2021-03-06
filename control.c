#include "fns.h"
#include "mpc.h"

static mpc_parser_t *
mpd_next(void)
{
	return mpd_cmd_noarg("next");
}

mpdf_fold(pause, mpd_opt_arg(MPD_ARG_BOOL))

static mpc_parser_t *
mpd_pause(void)
{
	/* TODO: usage without argument is deprecated */
	return mpc_and(2, mpdf_pause, mpc_string("pause"),
	               mpc_maybe(mpd_argument(mpd_binary())), free);
}

mpdf_fold(play, mpd_opt_arg(MPD_ARG_UINT))

static mpc_parser_t *
mpd_play(void)
{
	return mpc_and(2, mpdf_play, mpc_string("play"),
	               mpc_maybe(mpd_argument(mpd_uint())), free);
}

mpdf_fold(playid, mpd_opt_arg(MPD_ARG_UINT))

static mpc_parser_t *
mpd_playid(void)
{
	return mpc_and(2, mpdf_playid, mpc_string("playid"),
	               mpc_maybe(mpd_argument(mpd_uint())), free);
}

static mpc_parser_t *
mpd_previous(void)
{
	return mpd_cmd_noarg("previous");
}

mpdf_fold(seek, MPD_ARG_UINT MPD_ARG_FLOAT)

static mpc_parser_t *
mpd_seek(void)
{
	return mpc_and(3, mpdf_seek, mpc_string("seek"),
	               mpd_argument(mpd_uint()), mpd_argument(mpd_float()),
	               free, free);
}

mpdf_fold(seekid, MPD_ARG_UINT MPD_ARG_FLOAT)

static mpc_parser_t *
mpd_seekid(void)
{
	return mpc_and(3, mpdf_seekid, mpc_string("seekid"),
	               mpd_argument(mpd_uint()), mpd_argument(mpd_float()),
	               free, free);
}

/* TODO: Implement seekcur, requires distinguishing +n and n */

static mpc_parser_t *
mpd_stop(void)
{
	return mpd_cmd_noarg("stop");
}

mpc_parser_t *
mpd_control_cmds(void)
{
	return mpc_or(8, mpd_next(), mpd_pause(), mpd_play(), mpd_playid(),
	              mpd_previous(), mpd_seek(), mpd_seekid(), mpd_stop());
}

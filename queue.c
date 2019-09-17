#include "fns.h"
#include "mpc.h"

mpdf_fold(add, MPD_ARG_STRING)

static mpc_parser_t *
mpd_add(void)
{
	return mpc_and(2, mpdf_add, mpc_string("add"), mpd_argument(mpd_uri()),
	               free);
}

mpdf_fold(addid, MPD_ARG_STRING mpd_opt_arg(MPD_ARG_UINT))

static mpc_parser_t *
mpd_addid(void)
{
	return mpc_and(3, mpdf_addid, mpc_string("addid"),
	               mpd_argument(mpd_uri()),
	               mpc_maybe(mpd_argument(mpd_uint())), free, free);
}

static mpc_parser_t *
mpd_clear(void)
{
	return mpd_cmd_noarg("clear");
}

mpdf_fold(deleteid, MPD_ARG_UINT)

static mpc_parser_t *
mpd_deleteid(void)
{
	return mpc_and(2, mpdf_deleteid, mpc_string("deleteid"),
	               mpd_argument(mpd_uint()), free);
}

mpdf_fold(delete, MPD_ARG_RANGE)

static mpc_parser_t *
mpd_delete(void)
{
	return mpc_and(2, mpdf_delete, mpc_string("delete"),
	               mpd_argument(mpd_range_with_single()), free);
}

mpdf_fold(move, MPD_ARG_RANGE MPD_ARG_INT)

static mpc_parser_t *
mpd_move(void)
{
	return mpc_and(3, mpdf_move, mpc_string("move"),
	               mpd_argument(mpd_range_with_single()),
	               mpd_argument(mpd_int()), free, free);
}

mpdf_fold(moveid, MPD_ARG_UINT MPD_ARG_INT)

static mpc_parser_t *
mpd_moveid(void)
{
	return mpc_and(3, mpdf_moveid, mpc_string("moveid"),
	               mpd_argument(mpd_uint()), mpd_argument(mpd_int()), free,
	               free);
}

mpdf_fold(playlistfind, MPD_ARG_STRING MPD_ARG_STRING)

static mpc_parser_t *
mpd_playlistfind(void)
{
	return mpc_and(3, mpdf_playlistfind, mpc_string("playlistfind"),
	               mpd_argument(mpd_tag_name()), mpd_argument(mpd_string()),
	               free, free);
}

mpdf_fold(plchanges, MPD_ARG_UINT mpd_opt_arg(MPD_ARG_RANGE))

static mpc_parser_t *
mpd_plchanges(void)
{
	return mpc_and(3, mpdf_plchanges, mpc_string("plchanges"),
	               mpd_argument(mpd_uint()),
	               mpc_maybe(mpd_argument(mpd_range())), free, free);
}

mpdf_fold(rangeid, MPD_ARG_UINT MPD_ARG_RANGE)

static mpc_parser_t *
mpd_rangeid(void)
{
	return mpc_and(3, mpdf_rangeid, mpc_string("rangeid"),
	               mpd_argument(mpd_uint()), mpd_argument(mpd_range()),
	               free, free);
}

mpc_parser_t *
mpd_queue_cmds(void)
{
	return mpc_or(10, mpd_add(), mpd_addid(), mpd_clear(), mpd_delete(),
	              mpd_deleteid(), mpd_move(), mpd_moveid(),
	              mpd_playlistfind(), mpd_plchanges(), mpd_rangeid());
}

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
mpd_clear(void)
{
	return mpd_cmd_noarg("clear");
}

static mpc_parser_t *
mpd_addid(void)
{
	return mpc_and(3, mpdf_addid, mpc_string("addid"),
	               mpd_argument(mpd_uri()),
	               mpc_maybe(mpd_argument(mpd_uint())), free, free);
}

mpdf_fold(delete, MPD_ARG_RANGE)

static mpc_parser_t *
mpd_delete(void)
{
	return mpc_and(2, mpdf_delete, mpc_string("delete"),
	               mpd_argument(mpd_range_with_single()), free);
}

mpdf_fold(moveid, MPD_ARG_UINT MPD_ARG_INT)

static mpc_parser_t *
mpd_moveid(void)
{
	return mpc_and(3, mpdf_moveid, mpc_string("moveid"),
	               mpd_argument(mpd_uint()), mpd_argument(mpd_int()), free,
	               free);
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
	return mpc_or(6, mpd_add(), mpd_clear(), mpd_addid(), mpd_delete(),
	              mpd_moveid(), mpd_rangeid());
}

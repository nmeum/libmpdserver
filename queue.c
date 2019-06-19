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

mpdf_fold(moveid, MPD_ARG_UINT MPD_ARG_INT)

static mpc_parser_t *
mpd_moveid(void)
{
	mpc_parser_t *toval, *to;

	toval = mpc_or(2, mpc_and(2, mpcf_strfold, mpc_char('-'), mpc_digits()),
	               mpc_digits());
	to = mpc_expect(mpc_apply(toval, mpcf_int), "signed integer");

	return mpc_and(3, mpdf_moveid, mpc_string("moveid"),
	               mpd_argument(mpd_uint()), mpd_argument(to), free, free);
}

mpc_parser_t *
mpd_queue_cmds(void)
{
	return mpc_or(3, mpd_add(), mpd_delete(), mpd_moveid());
}

#include "fns.h"
#include "mpc.h"

mpdf_fold(count, MPD_ARG_EXPR mpd_opt_arg(MPD_ARG_STRING))

static mpc_parser_t *
mpd_count(void)
{
	mpc_parser_t *group;

	group = mpc_and(2, mpcf_snd_free, mpd_argument(mpc_string("group")),
	                mpd_argument(mpd_tag_name()), free);

	return mpc_and(3, mpdf_count, mpc_string("count"),
	               mpd_argument(mpd_string()), mpc_maybe(group), free,
	               free);
}

mpc_parser_t *
mpd_database_cmds(void)
{
	return mpd_count();
}

#include "fns.h"
#include "mpc.h"

static mpc_parser_t *
mpd_ping(void)
{
	return mpd_cmd_noarg("ping");
}

static mpc_parser_t *
mpd_tagtypes_clear(void)
{
	return mpc_apply(mpd_subcommand("tagtypes", "clear"),
	                 mpdf_command_noarg);
}

static mpc_parser_t *
mpd_tagtypes_all(void)
{
	return mpc_apply(mpd_subcommand("tagtypes", "all"), mpdf_command_noarg);
}

mpdf_fold_args(tagtypes_disable, "tagtypes disable", mpd_arg_many(MPD_ARG_STRING))

static mpc_parser_t *
mpd_tagtypes_disable(void)
{
	mpc_parser_t *cmd, *arg;

	/* TODO: use mpcf_freefold */
	cmd = mpc_and(2, mpcf_strfold, mpc_string("tagtypes"),
                      mpd_argument(mpc_string("disable")), free);
	arg = mpc_many1(mpdf_tagtypes_disable, mpd_argument(mpd_string()));

	return mpc_and(2, mpcf_snd_free, cmd, arg, free);
}

mpdf_fold_args(tagtypes_enable, "tagtypes enable", mpd_arg_many(MPD_ARG_STRING))

static mpc_parser_t *
mpd_tagtypes_enable(void)
{
	mpc_parser_t *cmd, *arg;

	/* TODO: use mpcf_freefold */
	cmd = mpc_and(2, mpcf_strfold, mpc_string("tagtypes"),
                      mpd_argument(mpc_string("enable")), free);
	arg = mpc_many1(mpdf_tagtypes_enable, mpd_argument(mpd_string()));

	return mpc_and(2, mpcf_snd_free, cmd, arg, free);
}

static mpc_parser_t *
mpd_tagtypes(void)
{
	return mpd_cmd_noarg("tagtypes");
}

mpc_parser_t *
mpd_connection_cmds(void)
{
	return mpc_or(6, mpd_ping(), mpd_tagtypes_all(), mpd_tagtypes_clear(),
	              mpd_tagtypes_disable(), mpd_tagtypes_enable(),
	              mpd_tagtypes());
}

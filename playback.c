#include "fns.h"
#include "mpc.h"

static char *mpd_gain_modes[] = {
    "off",
    "track",
    "album",
    "auto",
};

static mpc_val_t *
mpdf_nan(mpc_val_t *val)
{
	int *ptr;

	free(val);
	ptr = xmalloc(sizeof(*ptr));
	*ptr = -1;
	return ptr;
}

static int
mpd_check_volume(mpc_val_t **val)
{
	unsigned int *ptr;

	ptr = *(unsigned **)val;
	return *ptr <= 100;
}

mpdf_fold(consume, MPD_ARG_BOOL)

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

mpdf_fold(mixrampdb, MPD_ARG_FLOAT)

static mpc_parser_t *
mpd_mixrampdb(void)
{
	mpc_parser_t *neg, *val;

	neg = mpc_and(2, mpcf_strfold, mpc_char('-'), mpd_float_digits(), free);
	val = mpc_or(2, mpd_float(), mpc_apply(neg, mpcf_float));

	return mpc_and(2, mpdf_mixrampdb, mpc_string("mixrampdb"),
	               mpd_argument(val), free);
}

mpdf_fold(mixrampdelay, MPD_ARG_INT)

static mpc_parser_t *
mpd_mixrampdelay(void)
{
	mpc_parser_t *val;

	/* TODO: document that nan is canonicalized as -1 */
	val = mpc_or(2, mpc_int(), mpc_apply(mpc_string("nan"), mpdf_nan));
	return mpc_and(2, mpdf_mixrampdelay, mpc_string("mixrampdelay"),
	               mpd_argument(val), free);
}

mpdf_fold(random, MPD_ARG_BOOL)

static mpc_parser_t *
mpd_random(void)
{
	return mpc_and(2, mpdf_random, mpc_string("random"),
	               mpd_argument(mpd_binary()), free);
}

mpdf_fold(repeat, MPD_ARG_BOOL)

static mpc_parser_t *
mpd_repeat(void)
{
	return mpc_and(2, mpdf_repeat, mpc_string("repeat"),
	               mpd_argument(mpd_binary()), free);
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

mpdf_fold(single, MPD_ARG_STRING)

static mpc_parser_t *
mpd_single(void)
{
	mpc_parser_t *val;

	/* TODO: don't canonicalize value as string */
	val =
	    mpc_or(3, mpc_string("0"), mpc_string("1"), mpc_string("oneshot"));
	return mpc_and(2, mpdf_single, mpc_string("single"), mpd_argument(val),
	               free);
}

mpdf_fold(replay_gain_mode, MPD_ARG_STRING)

static mpc_parser_t *
mpd_replay_gain_mode(void)
{
	mpc_parser_t *mode;
	static mpd_string_array_t ary = MPD_STRING_ARY(mpd_gain_modes);

	mode = mpc_check_with(mpd_string_case(), free, mpd_check_array, &ary,
	                      "invalid gain mode");
	return mpc_and(2, mpdf_replay_gain_mode, mpc_string("replay_gain_mode"),
	               mpd_argument(mode), free);
}

static mpc_parser_t *
mpd_replay_gain_status(void)
{
	return mpd_cmd_noarg("replay_gain_status");
}

mpdf_fold(volume, MPD_ARG_INT)

static mpc_parser_t *
mpd_volume(void)
{
	/* XXX: This is deprecated */
	return mpc_and(2, mpdf_volume, mpc_string("volume"),
	               mpd_argument(mpd_int()), free);
}

mpc_parser_t *
mpd_playback_cmds(void)
{
	return mpc_or(11, mpd_consume(), mpd_crossfade(), mpd_mixrampdb(),
	              mpd_mixrampdelay(), mpd_setvol(), mpd_single(),
	              mpd_random(), mpd_repeat(), mpd_replay_gain_mode(),
	              mpd_replay_gain_status(), mpd_volume());
}

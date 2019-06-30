#include "fns.h"
#include "mpc.h"

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

mpc_parser_t *
mpd_playback_cmds(void)
{
	return mpc_or(7, mpd_consume(), mpd_crossfade(), mpd_mixrampdb(),
	              mpd_mixrampdelay(), mpd_setvol(), mpd_random(),
	              mpd_repeat());
}

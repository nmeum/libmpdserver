#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "fns.h"
#include "mpc.h"

static char *mpd_tag_names[] = {
    "any",
    "artist",
    "artistsort",
    "album",
    "albumsort",
    "albumartist",
    "albumartistsort",
    "title",
    "track",
    "name",
    "genre",
    "date",
    "composer",
    "performer",
    "grouping",
    "comment",
    "disc",
    "label",
    "musicbrainz_artistid",
    "musicbrainz_albumid",
    "musicbrainz_albumartistid",
    "musicbrainz_trackid",
    "musicbrainz_releasetrackid",
    "musicbrainz_workid",
};

int
mpd_check_tag_name(mpc_val_t **val)
{
	size_t i;
	char *str;
	int inset;

	/* TODO: duplication of mpd_check_subsys */

	str = *(char **)val;
	for (inset = 0, i = 0; i < LENGTH(mpd_tag_names); i++) {
		if (!strcmp(str, mpd_tag_names[i]))
			inset = 1;
	}

	return inset;
}

static mpc_parser_t *
mpd_expr_string(void)
{
	mpc_parser_t *strch, *par;

	strch = mpc_or(2, mpc_escape(), mpc_noneof("\'"));
	par = mpc_between(mpc_many(mpcf_strfold, strch), free, "'", "'");

	return mpc_apply(mpc_or(2, par, mpc_string_lit()), mpdf_unescape);
}

static mpc_val_t *
mpdf_fold_seperator(int n, mpc_val_t **xs)
{
	if (!n)
		return xstrdup("");

	return mpcf_fst_free(n, xs);
}

static mpc_parser_t *
mpd_seperator(void)
{
	return mpc_many(mpdf_fold_seperator, mpd_whitespace());
}

static mpc_parser_t *
mpd_op(mpc_parser_t *par)
{
	return mpc_and(3, mpcf_snd_free, mpd_seperator(), par, mpd_seperator(),
	               free, free);
}

static mpd_operation_t
mpd_str2op(char *str)
{
	if (!strcmp(str, "=="))
		return MPD_OP_EQUAL;
	else if (!strcmp(str, "!="))
		return MPD_OP_NEQUAL;
	else if (!strcmp(str, "contains"))
		return MPD_OP_CONTAINS;
	else if (!strcmp(str, "=~"))
		return MPD_OP_MATCH;
	else if (!strcmp(str, "!~"))
		return MPD_OP_NMATCH;
	else
		return MPD_OP_NONE;
}

static mpc_val_t *
mpdf_fold_expression(int n, mpc_val_t **xs)
{
	mpd_expression_t *expr;

	assert(n == 3);

	expr = xmalloc(sizeof(*expr));
	expr->name = (char *)xs[0];
	expr->op = mpd_str2op((char *)xs[1]);
	expr->o1.str = (char *)xs[2];
	expr->next = NULL;

	free(xs[1]);
	return expr;
}

static mpc_val_t *
mpdf_fold_expressions(int n, mpc_val_t **xs)
{
	mpd_expression_t *expr;

	expr = xmalloc(sizeof(*expr));
	expr->op = MPD_OP_NONE;

	if (n == 3) {
		expr->name = (char *)xs[1];
		assert(!strcmp(expr->name, "AND"));
		expr->o1.expr = (mpd_expression_t *)xs[0];
		expr->next = (mpd_expression_t *)xs[2];
	} else if (n == 2) {
		expr->name = xstrdup("NOT");
		assert(!strcmp((char *)xs[0], "!"));
		expr->o1.expr = (mpd_expression_t *)xs[1];
		expr->next = NULL;
		free(xs[0]);
	} else {
		assert(0);
	}

	return expr;
}

static mpc_parser_t *
mpd_tag(void)
{
	mpc_parser_t *str, *strcheck, *op;

	str = mpc_many1(mpcf_strfold,
	                mpc_or(2, mpc_range('a', 'z'), mpc_range('A', 'Z')));
	strcheck = mpc_check(str, free, mpd_check_tag_name, "invalid tag");

	op = mpc_or(5, mpc_string("=="), mpc_string("!="),
	            mpc_string("contains"), mpc_string("=~"), mpc_string("!~"));

	return mpc_and(3, mpdf_fold_expression, strcheck, mpd_op(op),
	               mpd_expr_string(), free, free);
}

static mpc_parser_t *
mpd_file(void)
{
	return mpc_and(3, mpdf_fold_expression, mpc_string("file"),
	               mpd_op(mpc_string("==")), mpd_expr_string(), free, free);
}

static mpc_parser_t *
mpd_base(void)
{
	return mpc_and(3, mpdf_fold_expression, mpc_string("base"),
	               mpd_seperator(), mpd_expr_string(), free, free);
}

static mpc_parser_t *
mpd_modified_since(void)
{
	return mpc_and(3, mpdf_fold_expression, mpc_string("modified-since"),
	               mpd_seperator(), mpd_expr_string(), free, free);
}

static mpc_parser_t *
mpd_audio_format(void)
{
	mpc_parser_t *ops;

	ops = mpc_or(2, mpc_string("=="), mpc_string("=~"));
	return mpc_and(3, mpdf_fold_expression, mpc_string("AudioFormat"),
	               mpd_op(ops), mpd_expr_string(), free, free);
}

static mpc_parser_t *
mpd_base_exprs(void)
{
	return mpc_or(5, mpd_file(), mpd_base(), mpd_modified_since(),
	              mpd_audio_format(), mpd_tag());
}

void
mpd_free_expression(void *ptr)
{
	mpd_expression_t *expr;

	expr = (mpd_expression_t *)ptr;
	if (!strcmp(expr->name, "AND") || !strcmp(expr->name, "NOT"))
		mpd_free_expression(expr->o1.expr);
	else
		free(expr->o1.str);

	if (expr->next)
		mpd_free_expression(expr->next);

	free(expr->name);
	free(expr);
}

mpd_expression_t *
mpd_expression(char *str)
{
	mpc_result_t r;
	mpc_parser_t *expr;
	mpd_expression_t *ret;

	expr = mpc_new("expr");
	mpc_define(expr,
	           mpc_between(mpc_or(3, mpd_base_exprs(),
	                              mpc_and(3, mpdf_fold_expressions, expr,
	                                      mpd_op(mpc_string("AND")), expr,
	                                      mpd_free_expression, free),
	                              mpc_and(2, mpdf_fold_expressions,
	                                      mpc_char('!'), expr, free)),
	                       mpd_free_expression, "(", ")"));

	if (mpc_parse("", str, expr, &r)) {
		ret = (mpd_expression_t *)r.output;
	} else {
		mpc_err_delete(r.error);
		ret = NULL;
	}

	mpc_cleanup(1, expr);
	return ret;
}

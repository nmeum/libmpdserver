#include <assert.h>
#include <stddef.h>

#include "fns.h"
#include "mpc.h"

static mpc_val_t *
mpdf_fold_command(int n, mpc_val_t **xs)
{
	int i;
	mpd_command_t *cmd;

	assert(n > 0);

	cmd = mpd_new_command(NULL, (size_t)n);
	for (i = 0; i < n; i++) {
		cmd->argv[i] = xmalloc(sizeof(mpd_argument_t));
		cmd->argv[i]->type = MPD_VAL_CMD;
		cmd->argv[i]->v.cmdval = (mpd_command_t *)xs[i];
	}

	return cmd;
}

static mpc_val_t *
mpdf_fold_list(int n, mpc_val_t **xs)
{
	mpd_command_t *cmd;
	char *lstart;

	assert(n == 3);
	assert(!strcmp(xs[n - 1], "command_list_end"));

	lstart = (char *)xs[0];
	assert(!strcmp(lstart, "command_list_begin") ||
	       !strcmp(lstart, "command_list_ok_begin"));

	free(xs[n - 1]);

	cmd = (mpd_command_t *)xs[1];
	assert(!cmd->name);
	cmd->name = lstart;

	return cmd;
}

mpc_parser_t *
mpd_list_cmds(void)
{
	mpc_parser_t *endstr, *startstrs, *lbeg, *lend, *cmds;

	startstrs = mpc_or(2, mpc_string("command_list_begin"),
	                   mpc_string("command_list_ok_begin"));
	endstr = mpc_string("command_list_end");

	lbeg = mpc_and(2, mpcf_fst_free, startstrs, mpc_newline(), free);
	lend = mpc_and(2, mpcf_fst_free, endstr, mpc_newline(), free);

	cmds = mpc_many1(mpdf_fold_command, mpd_command_primitive());
	return mpc_and(3, mpdf_fold_list, lbeg, cmds, lend, free,
	               mpd_free_command);
}

#include <stdio.h>
#include <stdlib.h>

#include "mpdserver.h"

int
main(void)
{
	int ret;
	mpd_command_t *cmd;

	if ((cmd = mpd_parse(stdin))) {
		mpd_free_command(cmd);
		ret = EXIT_SUCCESS;
	} else {
		ret = EXIT_FAILURE;
	}

	return ret;
}

#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpdserver.h"

static void format_cmd(int, mpd_command_t *);

static char *
format_op(mpd_operation_t op)
{
	switch (op) {
	case MPD_OP_NONE:
		return "";
	case MPD_OP_EQUAL:
		return "==";
	case MPD_OP_NEQUAL:
		return "!=";
	case MPD_OP_CONTAINS:
		return "contains";
	case MPD_OP_MATCH:
		return "=~";
	case MPD_OP_NMATCH:
		return "!~";
	default:
		assert(0);
	}
}

static char *
format_val(mpd_val_t val)
{
	switch (val) {
	case MPD_VAL_INT:
		return "int";
	case MPD_VAL_UINT:
		return "uint";
	case MPD_VAL_STR:
		return "str";
	case MPD_VAL_FLOAT:
		return "float";
	case MPD_VAL_BOOL:
		return "bool";
	case MPD_VAL_RANGE:
		return "range";
	case MPD_VAL_EXPR:
		return "expr";
	case MPD_VAL_CMD:
		return "cmd";
	default:
		assert(0);
		return NULL;
	}
}

static void
format_expr(int ident, mpd_expression_t *expr)
{
	printf("%*sname: %s\n", ident, "", expr->name);
	printf("%*soperation: %s\n", ident, "", format_op(expr->op));

	printf("%*sargument: ", ident, "");
	if (!strcmp(expr->name, "AND") || !strcmp(expr->name, "NOT")) {
		printf("\n");
		format_expr(ident + 2, expr->o1.expr);
	} else {
		printf("%s\n", expr->o1.str);
	}

	if (expr->next) {
		printf("%*snext:\n", ident, "");
		format_expr(ident + 2, expr->next);
	}
}

static void
format_arg(int ident, mpd_argument_t *arg)
{
	switch (arg->type) {
	case MPD_VAL_INT:
		printf("%d\n", arg->v.ival);
		break;
	case MPD_VAL_UINT:
		printf("%u\n", arg->v.uval);
		break;
	case MPD_VAL_STR:
		printf("%s\n", arg->v.sval);
		break;
	case MPD_VAL_FLOAT:
		printf("%.1f\n", arg->v.fval);
		break;
	case MPD_VAL_BOOL:
		printf("%s\n", (arg->v.bval) ? "true" : "false");
		break;
	case MPD_VAL_RANGE:
		printf("%zu:%zd\n", arg->v.rval.start, arg->v.rval.end);
		break;
	case MPD_VAL_EXPR:
		printf("\n");
		format_expr(ident + 2, arg->v.eval);
		break;
	case MPD_VAL_CMD:
		printf("\n");
		format_cmd(ident + 2, arg->v.cmdval);
		break;
	default:
		errx(EXIT_FAILURE, "unsupported type: %u\n", arg->type);
		break;
	}
}

static void
format_cmd(int ident, mpd_command_t *cmd)
{
	size_t i;
	mpd_argument_t *arg;

	printf("%*sname: %s\n", ident, "", cmd->name);
	if (!cmd->argc)
		printf("%*sargv: []\n", ident, "");
	else
		printf("%*sargv:\n", ident, "");

	ident += 2;
	for (i = 0; i < cmd->argc; i++) {
		arg = cmd->argv[i];

		printf("%*s- type: %s\n", ident, "", format_val(arg->type));
		ident += 2;

		printf("%*svalue: ", ident, "");
		format_arg(ident, arg);

		ident -= 2;
	}
}

static bool
is_lststart(char *str)
{
	return !strcmp(str, "command_list_begin\n") ||
	       !(strcmp(str, "command_list_ok_begin\n"));
}

static bool
is_lstend(char *str)
{
	return !strcmp(str, "command_list_end\n");
}

static char *
read_command(FILE *stream)
{
	size_t nlen;
	static size_t llen;
	static char *line, *result;

	while (getline(&line, &llen, stream) != -1) {
		if (!result) {
			if (!(result = strdup(line)))
				err(EXIT_FAILURE, "strdup failed");

			if (is_lststart(line))
				continue;
			else
				break;
		}

		nlen = strlen(result) + strlen(line) + 1;
		if (!(result = realloc(result, nlen)))
			err(EXIT_FAILURE, "realloc failed");
		strncpy(&result[strlen(result)], line, nlen - strlen(result));

		if (is_lstend(line))
			break;
	}

	free(line);
	if (ferror(stdin))
		err(EXIT_FAILURE, "ferror failed");

	return result;
}

int
main(void)
{
	int ret;
	char *input;
	mpd_command_t *cmd;

	input = read_command(stdin); /* NULL if empty */
	if (!(cmd = mpd_parse((input) ? input : ""))) {
		ret = EXIT_FAILURE;
		goto exit;
	}

	format_cmd(0, cmd);
	mpd_free_command(cmd);

	ret = EXIT_SUCCESS;
exit:
	free(input);
	return ret;
}

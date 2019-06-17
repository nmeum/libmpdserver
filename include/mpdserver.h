#ifndef MPDSERVER_H
#define MPDSERVER_H

#include <stdio.h>

#include <sys/types.h>

typedef enum {
	MPD_VAL_INT,
	MPD_VAL_UINT,
	MPD_VAL_STR,
	MPD_VAL_FLOAT,
	MPD_VAL_RANGE,
	MPD_VAL_EXPR,
	MPD_VAL_CMD,

	MPD_VAL_EXPR_STR, /* only used internally */
} mpd_val_t;

typedef enum {
	MPD_OP_NONE,     /* base, modified-since, AND and NOT */
	MPD_OP_EQUAL,    /* == */
	MPD_OP_NEQUAL,   /* != */
	MPD_OP_CONTAINS, /* contains (tag only) */
	MPD_OP_MATCH,    /* =~ */
	MPD_OP_NMATCH,   /* !~ (tag only) */
} mpd_operation_t;

typedef struct _mpd_argument_t mpd_argument_t;

typedef struct {
	char *name;
	size_t argc;
	mpd_argument_t **argv;
} mpd_command_t;

typedef struct _mpd_expression_t mpd_expression_t;

/* This data structure is a bit clumsy and might change in the future */
struct _mpd_expression_t {
	char *name;
	mpd_operation_t op;

	union {
		char *str;
		mpd_expression_t *expr;
	} o1;
	mpd_expression_t *next; /* only used by AND */
};

typedef struct {
	size_t start;
	ssize_t end; /* -1 if end is omitted */
} mpd_range_t;

struct _mpd_argument_t {
	mpd_val_t type;
	union {
		int ival;
		unsigned int uval;
		char *sval;
		float fval;
		mpd_range_t rval;
		mpd_expression_t *eval;
		mpd_command_t *cmdval;
	} v;
};

mpd_command_t *mpd_parse(FILE *);
void mpd_free_command(mpd_command_t *);

#endif

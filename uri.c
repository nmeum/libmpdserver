#include <stdlib.h>
#include <stdio.h>
#include "mpc.h"

int
main(void)
{
	mpc_result_t r;

	mpc_parser_t *URI = mpc_new("URI");
	mpc_parser_t *hier_part = mpc_new("hier_part");
	mpc_parser_t *scheme = mpc_new("scheme");
	mpc_parser_t *authority = mpc_new("authority");
	mpc_parser_t *userinfo = mpc_new("userinfo");
	mpc_parser_t *host = mpc_new("host");
	mpc_parser_t *port = mpc_new("port");
	mpc_parser_t *reg_name = mpc_new("reg_name");
	mpc_parser_t *path_abempty = mpc_new("path_abempty");
	mpc_parser_t *segment = mpc_new("segment");
	mpc_parser_t *pchar = mpc_new("pchar");
	mpc_parser_t *query = mpc_new("query");
	mpc_parser_t *fragment = mpc_new("fragment");
	mpc_parser_t *pct_encoded = mpc_new("pct_encoded");
	mpc_parser_t *unreserved = mpc_new("unreserved");
	mpc_parser_t *sub_delims = mpc_new("sub_delims");
	mpc_parser_t *digit = mpc_new("digit");
	mpc_parser_t *alpha = mpc_new("alpha");

	mpc_err_t* err = mpca_lang(MPCA_LANG_DEFAULT,
		" URI          : /^/ <scheme> \":\" <hier_part> ( \"?\" <query> )? ( \"#\" <fragment> )? /$/ ;\n"
		" hier_part    : \"//\" <authority> <path_abempty> ;\n" /* TODO */

		" scheme       : <alpha> ( <alpha> | <digit> | \"+\" | \"-\" | \".\" )* ;\n"

		" authority    : ( <userinfo> \"@\" )? <host> ( \":\" <port> )? ;\n"
		" userinfo     : ( <unreserved> | <pct_encoded> | <sub_delims> | \":\" )* ;\n"
		" host         : <reg_name> ;\n" /* TODO: IP addresses */
		" port         : <digit>* ;\n"

		" reg_name     : ( <unreserved> | <pct_encoded> | <sub_delims> )* ;\n"

		" path_abempty : ( \"/\" <segment> )* ;\n"

		" segment      : <pchar>* ;\n"

		" pchar        : <unreserved> | <pct_encoded> | <sub_delims> | \":\" | \"@\" ;\n"
		" query        : ( <pchar> | \"/\" | \"?\" )* ;\n"
		" fragment     : ( <pchar> | \"/\" | \"?\" )* ;\n"

		" pct_encoded  : \"%\" <hexdig> <hexdig> ;\n"
		" unreserved   : <alpha> | <digit> | \"-\" | \".\" | \"_\" | \"~\" ;\n"
		" sub_delims   : \"!\" | \"$\" | \"&\" | \"'\" | \"(\" | \")\" \n"
		"              | \"*\" | \"+\" | \",\" | \";\" | \"=\" ;\n"

		" hexdig       : <digit> | \"A\" | \"B\" | \"C\" | \"D\" | \"E\" | \"F\" ;\n"
		" digit        : /[0-9]/ ;\n"
		" alpha        : /[a-zA-Z]/ ;\n",
	URI,
	hier_part,
	scheme,
	authority,
	userinfo,
	host,
	port,
	reg_name,
	path_abempty,
	segment,
	pchar,
	query,
	fragment,
	pct_encoded,
	unreserved,
	sub_delims,
	digit,
	alpha, NULL);


  if (err != NULL) {
    mpc_err_print(err);
    mpc_err_delete(err);
    return EXIT_FAILURE;
  }

  if (mpc_parse("file", "http://google.com/foo", URI, &r)) {
	mpc_ast_print(r.output);
	mpc_ast_delete(r.output);
  } else {
  	  mpc_err_print(r.error);
  	  mpc_err_delete(r.error);
  }

  return EXIT_SUCCESS;
}

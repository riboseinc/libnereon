/*
 * Copyright (c) 2018, [Ribose Inc](https://www.ribose.com).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <ucl.h>

#include "err.h"
#include "util.h"

#include "nereon.h"

#if 0

/*
 * show helper message
 */

void nereon_cli_print_usage(struct nereon_nos_option *nos_opts, int nos_opts_count)
{
	int max_sw_len = 0, max_desc_len = 0;
	int i;

	/* get max len */
	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_option *opt = &nos_opts[i];

		max_sw_len = max_sw_len > strlen(opt->sw_long) ? max_sw_len : strlen(opt->sw_long);
		max_desc_len = max_desc_len > strlen(opt->desc_short) ? max_desc_len : strlen(opt->desc_short);
	}

	fprintf(stdout, "Usage: %s [options]\n", getprogname_s());
	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_option *opt = &nos_opts[i];

		char sw_short[CFG_MAX_SHORT_SWITCH];

		char *padding_sw, *padding_desc;
		int padding_sw_len, padding_desc_len;

		char *cli_lopt_prefix = strlen(opt->sw_short) > 0 ? "|" : "";

		if (strlen(opt->sw_short) == 0) {
			memset(sw_short, 0x20, sizeof(sw_short));
			sw_short[CFG_MAX_SHORT_SWITCH - 1] = '\0';
		} else {
			strcpy_s(sw_short, opt->sw_short, sizeof(sw_short));
		}

		padding_sw_len = strlen(opt->sw_long) > 0 ? max_sw_len - strlen(opt->sw_long) :
					max_sw_len + strlen(cli_lopt_prefix) - 1;
		padding_sw = fill_bytes(' ', padding_sw_len);

		padding_desc_len = strlen(opt->desc_short) > 0 ? max_desc_len - strlen(opt->desc_short) :
					max_sw_len + 1;
		padding_desc = fill_bytes(' ', max_desc_len - strlen(opt->desc_short));

		fprintf(stdout, "  %s%s%s%s %s%s: %s\n",
			sw_short,
			strlen(opt->sw_long) > 0 ? cli_lopt_prefix : " ",
			opt->sw_long, padding_sw,
			opt->desc_short,
			padding_desc,
			opt->desc_long);

		if (padding_sw)
			free(padding_sw);

		if (padding_desc)
			free(padding_desc);
	}
}

/*
 * set options from command line
 */

static int set_opt_val(const char *arg, struct nereon_nos_option *opt)
{
	if (opt->type == NEREON_TYPE_INT) {
		int i;

		i = (int)strtol(arg, NULL, 10);
		if (errno == EINVAL || errno == ERANGE) {
			return -1;
		}
		opt->data.i = i;

		DEBUG_PRINT("setting value '%d' for option '%s'\n", i, opt->name);
	} else if (opt->type == NEREON_TYPE_STRING || opt->type == NEREON_TYPE_IPPORT ||
		opt->type == NEREON_TYPE_CONFIG) {
		char *str;

		str = strdup(arg);
		if (!str) {
			return -1;
		}
		opt->data.str = str;

		DEBUG_PRINT("setting value '%s' for option '%s'\n", str, opt->name);
	}
	opt->is_cli_set = true;

	return 0;
}

/*
 * parse command line
 */

int nereon_cli_parse(struct nereon_nos_option *nos_opts, int nos_opts_count, int argc, char **argv, bool *require_exit)
{
	int i;

	if (argc == 1)
		return 0;

	DEBUG_PRINT("Parsing command line arguments\n");

	for (i = 1; i < argc; i++) {
		struct nereon_nos_option *opt;
		bool is_sw_short = false;

		/* get NOS option for switch */
		opt = nereon_get_nos_by_switch(nos_opts, nos_opts_count, argv[i]);
		if (!opt) {
			DEBUG_PRINT("Invalid command line switch '%s'\n", argv[i]);
			nereon_set_err("Invalid command line switch '%s'", argv[i]);

			return -1;
		}

		DEBUG_PRINT("Found NOS option with name:%s, sw_short:%s, sw_long:%s, type:%d\n",
				opt->name, opt->sw_short, opt->sw_long, opt->type);

		if (opt->type == NEREON_TYPE_HELPER) {
			DEBUG_PRINT("Help option is enabled\n");
			*require_exit = true;

			return 0;
		}

		if (opt->type == NEREON_TYPE_BOOL) {
			DEBUG_PRINT("Setting 'true' to option '%s'\n", opt->name);

			opt->data.b = true;
			opt->is_cli_set = true;

			continue;
		}

		/* set value for */
		i++;
		if (i == argc) {
			nereon_set_err("Missing argument for switch '%s'", argv[i - 1]);
			return -1;
		}

		if (set_opt_val(argv[i], opt) != 0) {
			nereon_set_err("Invalid argument for switch '%s'", argv[i - 1]);
			return -1;
		}
		opt->is_cli_set = true;
	}

	return 0;
}

#endif

/*
 * add argument to cli option
 */

static void add_arg_to_cli_option(nereon_cli_option_t *cli_opt, const char *arg)
{

}

/*
 * create new CLI option
 */

static nereon_cli_option_t *new_cli_option(nereon_cli_option_t *root_opt, nereon_nos_option_t *nos_opt)
{
	nerelon_cli_option_t *opt;

	/* allocate memory for cli option */
	opt = (nereon_cli_option_t *)malloc(sizeof(nereon_cli_option_t));
	if (!opt)
		return NULL;
	memset(opt, 0, sizeof(nereon_cli_option_t));

	opt->nos_opt = nos_opt;

	/* add cli option to root */
	if (!root_opt->next)
		root_opt->next = opt;
	else {
		nereon_cli_option_t *p = root_opt->next;

		while (p->next) {
			p = p->next;
		}
		p->next = opt;
	}

	return opt;
}

/*
 * add argument to cli option
 */

static int add_arg_to_cli_option(nereon_cli_option_t *cli_opt, const char *arg)
{
	cli_opt->args = realloc(cli_opt->args, (cli_opt->args_count + 1) * sizeof(char *));
	if (!cli_opt->args)
		return -1;
	cli_opt->args[cli_opt->args_count++] = arg;

	return 0;
}

/*
 * free cli option
 */

static void free_cli_option(nereon_cli_option_t *opt)
{
	if (opt->next)
		free_cli_option(opt->next);

	if (opt->args)
		free(opt->args);

	free(opt);
}

/*
 * free cli options
 */

void nereon_cli_free(nereon_cli_option_t *root_opt)
{
	free_cli_option(root_opt->next);
}

/*
 * parse command line arguments based on NOS schema
 */

int nereon_cli_parse(nereon_ctx_priv_t *ctx, int argc, char **argv, bool *require_exit)
{
	nereon_nos_schema_t *nos_schema = &ctx->nos_schema;
	nereon_nos_option_t *nos_opt = NULL;

	int i = 1, ret = 0;

	/* set command line options */
	while (ret == 0) {
		nereon_cli_option_t *cli_opt;

		/* get NOS option by switch */
		if (!nos_opt) {
			nos_opt = nereon_get_nos_by_sw(nos_schema, argv[i]);
			if (!nos_opt) {
				DEBUG_PRINT("CLI: Unknown option '%s'\n", argv[i]);
				ret = -1;
				break;
			}
		}

		/* create new cli option */
		cli_opt = new_cli_option(&ctx->root_cli_opt, nos_opt);
		if (!cli_opt) {
			ret = -1;
			break;
		}

		/* check if followed options are switch or arguments */
		do {
			if (++i == argc)
				break;

			nos_opt = nereon_get_nos_by_sw(nos_schema, argv[i]);
			if (nos_opt)
				break;

			if (add_arg_to_cli_option(cli_opt, argv[i]) != 0) {
				DEBUG_PRINT("CLI: Out of memory!\n");
				ret = -1;
				break;
			}
		} while (true);
	}

	if (ret != 0)
		nereon_cli_free(&ctx->root_cli_opt);

	return ret;
}

/*
 * Copyright (c) 2017, [Ribose Inc](https://www.ribose.com).
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
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <ucl.h>

#include "common.h"
#include "err.h"
#include "util.h"

#include "nereon.h"
#include "nos.h"

/*
 * show helper message
 */

void nereon_cli_print_usage(struct nereon_nos_options *nos_opts, int nos_opts_count)
{
	int max_sw_len = 0, max_desc_len = 0;
	int i;

	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_options *opt = &nos_opts[i];

		max_sw_len = max_sw_len > strlen(opt->sw_long) ? max_sw_len : strlen(opt->sw_long);
		max_desc_len = max_desc_len > strlen(opt->desc_short) ? max_desc_len : strlen(opt->desc_short);
	}

	max_sw_len += 2;
	max_desc_len += 2;

	DEBUG_PRINT("max_sw_len:%d, max_desc_len:%d\n", max_sw_len, max_desc_len);

	fprintf(stdout, "Usage: %s [options]\n", getprogname());
	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_options *opt = &nos_opts[i];

		char sw_short[3];

		char *padding_sw, *padding_desc;
		int padding_sw_len, padding_desc_len;

		char *cli_lopt_prefix = strlen(opt->sw_short) > 0 ? "|--" : "--";
		char *short_desc_prefix = opt->exist_cli_default ? "[" : "<";
		char *short_desc_tail = opt->exist_cli_default ? "]" : ">";

		if (strlen(opt->sw_short) == 0) {
			memset(sw_short, 0x20, sizeof(sw_short));
			sw_short[2] = '\0';
		} else {
			snprintf(sw_short, sizeof(sw_short), "-%s", opt->sw_short);
		}

		padding_sw_len = strlen(opt->sw_long) > 0 ? max_sw_len - strlen(opt->sw_long) :
					max_sw_len + strlen(cli_lopt_prefix) - 1;
		padding_sw = fill_bytes(' ', padding_sw_len);

		padding_desc_len = strlen(opt->desc_short) > 0 ? max_desc_len - strlen(opt->desc_short) :
					max_sw_len + 1;
		padding_desc = fill_bytes(' ', max_desc_len - strlen(opt->desc_short));

		if (opt->type != NEREON_TYPE_BOOL) {
			fprintf(stdout, "  %s%s%s%s%s%s%s%s: %s\n",
				sw_short,
				strlen(opt->sw_long) > 0 ? cli_lopt_prefix : " ",
				opt->sw_long, padding_sw,
				strlen(opt->desc_short) > 0 ? short_desc_prefix : " ",
				opt->desc_short,
				strlen(opt->desc_short) > 0 ? short_desc_tail : " ",
				padding_desc,
				opt->desc_long);
		} else {
			fprintf(stdout, "  %s%s%s%s%s  : %s\n",
				sw_short,
				strlen(opt->sw_long) > 0 ? cli_lopt_prefix : " ",
				opt->sw_long, padding_sw,
				padding_desc,
				opt->desc_long);
		}

		if (padding_sw)
			free(padding_sw);

		if (padding_desc)
			free(padding_desc);
	}
}

/*
 * set options from command line
 */

static int set_opt_val(const char *arg, struct nereon_nos_options *opt)
{
	if (opt->type == NEREON_TYPE_INT) {
		int i;

		i = (int)strtol(arg, NULL, 10);
		if (errno == EINVAL || errno == ERANGE) {
			return -1;
		}
		opt->data.i = i;
	} else if (opt->type == NEREON_TYPE_STRING || opt->type == NEREON_TYPE_IPPORT) {
		char *str;

		str = strdup(arg);
		if (!str) {
			return -1;
		}
		opt->data.str = str;
	}
	opt->is_set = true;

	return 0;
}

/*
 * parse command line
 */

int nereon_cli_parse(struct nereon_nos_options *nos_opts, int nos_opts_count, int argc, char **argv)
{
	int i;

	char sw_short[2];
	char sw_long[CFG_MAX_LONG_SWITCH];

	if (argc == 1)
		return 0;

	DEBUG_PRINT("Parsing command line arguments\n");

	for (i = 1; i < argc; i++) {
		int opt_idx;
		bool found_opt = false;

		sw_short[0] = sw_long[0] = '\0';

		if (*(argv[i]) == '-' && *(argv[i] + 1) != '-' && strlen(argv[i]) == 2) {
			strlcpy(sw_short, argv[i] + 1, sizeof(sw_short));
		} else if (strlen(argv[i]) > 2 && strncmp(argv[i], "--", 2) == 0) {
			strlcpy(sw_long, argv[i] + 2, sizeof(sw_long));
		} else {
			nereon_set_err("Invalid command line option '%s'", argv[i]);
			return -1;
		}

		DEBUG_PRINT("short_switch: %s, long_switch:%s\n", sw_short, sw_long);

		for (opt_idx = 0; opt_idx < nos_opts_count; opt_idx++) {
			struct nereon_nos_options *opt = &nos_opts[opt_idx];

			if (strcmp(sw_short, opt->sw_short) != 0 && strcmp(sw_long, opt->sw_long) != 0) {
				continue;
			}

			if (opt->type == NEREON_TYPE_BOOL) {
				opt->data.b = true;
				break;
			}

			i++;
			if (i == argc) {
				nereon_set_err("Missing argument for switch '%s'", argv[i - 1]);
				return -1;
			}

			if (set_opt_val(argv[i], opt) != 0) {
				nereon_set_err("Invalid argument for switch '%s'", argv[i - 1]);
				return -1;
			}
			found_opt = true;
		}

		if (!found_opt) {
			nereon_set_err("Invalid command line switch '%s'", argv[i]);
			return -1;
		}
	}

	return 0;
}

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
#include <stdbool.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>
#include <ucl.h>

#include "common.h"
#include "err.h"
#include "util.h"

#include "hcl.h"

/*
 * show helper message
 */

static void show_helper(const char *prog_name, struct mconfig_hcl_options *hcl_opts, int hcl_opts_count)
{
	int max_sw_len = 0, max_desc_len = 0;
	int i;

	for (i = 0; i < hcl_opts_count; i++) {
		struct mconfig_hcl_options *opt = &hcl_opts[i];

		max_sw_len = max_sw_len > strlen(opt->sw_long) ? max_sw_len : strlen(opt->sw_long);
		max_desc_len = max_desc_len > strlen(opt->desc_short) ? max_desc_len : strlen(opt->desc_short);
	}

	max_sw_len += 2;
	max_desc_len += 2;

	fprintf(stdout, "Usage: %s [options]\n", prog_name);
	for (i = 0; i < hcl_opts_count; i++) {
		struct mconfig_hcl_options *opt = &hcl_opts[i];

		char *padding_sw, *padding_desc;

		padding_sw = (char *)calloc(' ', max_sw_len - strlen(opt->sw_long) + 1);
		if (!padding_sw)
			return;

		padding_desc = (char *)calloc(' ', max_desc_len - strlen(opt->desc_short));
		if (!padding_desc) {
			free(padding_sw);
			return;
		}

		if (opt->cfg_type != CFG_TYPE_BOOL) {
			fprintf(stdout, "  -%s|--%s%s<%s>%s: %s\n",
				opt->sw_short,
				opt->sw_long, padding_sw,
				opt->desc_short, padding_desc,
				opt->desc_long);
		} else {
			fprintf(stdout, "  -%s|--%s%s%s  : %s\n",
				opt->sw_short,
				opt->sw_long, padding_sw,
				padding_desc,
				opt->desc_long);
		}

		free(padding_sw);
		free(padding_desc);
	}
}

/*
 * set options from command line
 */

int set_opt_val(const char *arg, struct mconfig_hcl_options *opt)
{
	if (opt->cfg_type == CFG_TYPE_INT) {
		int cfg_i;

		cfg_i = (int)strtol(arg, NULL, 10);
		if (errno == EINVAL || errno == ERANGE) {
			return -1;
		}
		opt->cfg_data.i = cfg_i;
	} else if (opt->cfg_type == CFG_TYPE_STRING || opt->cfg_type == CFG_TYPE_IPPORT) {
		char *cfg_str;

		cfg_str = strdup(arg);
		if (!cfg_str) {
			return -1;
		}
		opt->cfg_data.str = cfg_str;
	}

	return 0;
}

/*
 * parse command line
 */

int mconfig_parse_cmdline(struct mconfig_hcl_options *hcl_opts, int hcl_opts_count, char **args, int args_count)
{
	int i;

	char sw_short[2];
	char sw_long[CFG_MAX_LONG_SWITCH];

	if (args_count == 1)
		return 0;

	DEBUG_PRINT("Parsing command line arguments\n");

	for (i = 1; i < args_count; i++) {
		int opt_idx;

		sw_short[0] = sw_long[0] = '\0';

		if (*(args[i]) != '-' && strlen(args[i]) == 2) {
			strlcpy(sw_short, args[i] + 1, sizeof(sw_short));
		} else if (strlen(args[i]) > 2 && strncmp(args[i], "--", 2) == 0) {
			strlcpy(sw_long, args[i] + 2, sizeof(sw_long));
		} else {
			mconfig_set_err("Invalid command line option '%s'", args[i]);
			return -1;
		}

		for (opt_idx = 0; opt_idx < hcl_opts_count; opt_idx++) {
			struct mconfig_hcl_options *opt = &hcl_opts[opt_idx];

			if (strcmp(sw_short, opt->sw_short) != 0 && strcmp(sw_long, opt->sw_long) != 0) {
				mconfig_set_err("Invalid command line switch '%s'", args[i]);
				return -1;
			}

			if (opt->cfg_type == CFG_TYPE_BOOL) {
				if (opt->helper) {
					show_helper(args[0], hcl_opts, hcl_opts_count);
					return 0;
				}

				opt->cfg_data.b = true;
				break;
			}

			i++;
			if (i == args_count) {
				mconfig_set_err("Missing argument for switch '%s'", args[i - 1]);
				return -1;
			}

			if (set_opt_val(args[i], opt) != 0) {
				mconfig_set_err("Invalid argument for switch '%s'", args[i - 1]);
				return -1;
			}
		}
	}

	return 0;
}

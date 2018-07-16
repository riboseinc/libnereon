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

#include "nereon_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "err.h"
#include "util.h"
#include "nereon.h"

#include "nos.h"
#include "cli.h"
#include "noc.h"

/*
 * Intiialize libnereon context object
 */

int nereon_ctx_init(nereon_ctx_t *ctx, const char *nos_cfg)
{
	struct nereon_nos_option *nos_opts = NULL;
	int nos_opts_count = 0;

	struct nereon_cfg_options *cfg_opts = NULL;

	DEBUG_PRINT("Initializing libnereon context\n");

	memset(ctx, 0, sizeof(nereon_ctx_t));

	/* parse NOS options */
	if (nos_cfg && nereon_parse_nos_options(nos_cfg, &nos_opts, &nos_opts_count) != 0) {
		DEBUG_PRINT("Failed to parse NOS data(err:%s)\n", nereon_get_err());
		return -1;
	}

	ctx->nos_opts = (void *)nos_opts;
	ctx->nos_opts_count = nos_opts_count;

	ctx->noc_opts = cfg_opts;

	return 0;
}

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *ctx)
{
	struct nereon_nos_option *nos_opts = (struct nereon_nos_option *)ctx->nos_opts;
	struct nereon_noc_option *noc_opts = (struct nereon_noc_option *)ctx->noc_opts;

	nereon_free_noc_options(noc_opts);
	nereon_free_nos_options(nos_opts, ctx->nos_opts_count);
}

/*
 * get libnereon version
 */

const char *nereon_get_version_info(void)
{
	static char version_info[128];

	snprintf(version_info, sizeof(version_info), "version:%s, git commit:%s",
			STRINGIZE_VALUE_OF(LIBNEREON_VERSION), STRINGIZE_VALUE_OF(GIT_COMMIT_HASH));

	return version_info;
}

/*
 * parse nereon configuration options
 */

int nereon_get_config_options(nereon_ctx_t *ctx, nereon_config_option_t *cfg_opts, int opts_count)
{
	struct nereon_nos_option *nos_opts = (struct nereon_nos_option *)ctx->nos_opts;
	struct nereon_noc_option *noc_opts = (struct nereon_noc_option *)ctx->noc_opts;

	int i;

	for (i = 0; i < opts_count; i++) {
		nereon_config_option_t *cfg_opt = &cfg_opts[i];

		struct nereon_nos_option *nos_opt;
		struct nereon_noc_option *noc_opt;

		union nereon_config_data *cfg_data = NULL;

		/* get config from NOS */
		nos_opt = nereon_get_nos_option(nos_opts, ctx->nos_opts_count, cfg_opt->name);
		if (!nos_opt) {
			DEBUG_PRINT("Could not get NOS option for config '%s'\n", cfg_opt->name);
			return -1;
		}

		if (nos_opt->is_set) {
			DEBUG_PRINT("NOS option was set for config '%s'\n", cfg_opt->name);

			/* set config value from command line at first */
			cfg_data = &nos_opt->data;
		} else {
			/* set config value from configuration file */
			if (strlen(nos_opt->noc_key) > 0 &&
				(noc_opt = nereon_get_noc_option(noc_opts, nos_opt->noc_key))) {
				cfg_data = &noc_opt->data;
			} else if (nos_opt->exist_default) {
				DEBUG_PRINT("Setting NOS default data\n");
				cfg_data = &nos_opt->default_data;
			}
		}

		if (!cfg_data) {
			if (cfg_opt->mandatory) {
				nereon_set_err("Could not get configuration for '%s'", cfg_opt->name);
				return -1;
			}

			continue;
		}

		switch (cfg_opt->type) {
		case NEREON_TYPE_INT:
			*(int *)(cfg_opt->data) = cfg_data->i;
			break;

		case NEREON_TYPE_BOOL:
		case NEREON_TYPE_HELPER:
			*(bool *)(cfg_opt->data) = cfg_data->b;
			break;

		case NEREON_TYPE_STRING:
		case NEREON_TYPE_ARRAY:
		case NEREON_TYPE_IPPORT:
		case NEREON_TYPE_CONFIG:
			*(char **)(cfg_opt->data) = cfg_data->str;
			break;

		default:
			break;
		}
	}

	return 0;
}

/*
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *ctx, int argc, char **argv, bool *require_exit)
{
	struct nereon_nos_option *nos_opts = (struct nereon_nos_option *)ctx->nos_opts;

	return nereon_cli_parse(nos_opts, ctx->nos_opts_count, argc, argv, require_exit);
}

/*
 * parse NOC configuration file
 */

int nereon_parse_config_file(nereon_ctx_t *ctx, const char *noc_cfg_fpath)
{
	struct nereon_nos_option *nos_opts = (struct nereon_nos_option *)ctx->nos_opts;
	int i, ret;

	const char *cfg_fpath = noc_cfg_fpath;

	struct nereon_noc_option *noc_opts = NULL;

	/* check if configuration file was specified in command line */
	for (i = 0; i < ctx->nos_opts_count; i++) {
		struct nereon_nos_option *opt = &ctx->nos_opts[i];

		if (opt->type == NEREON_TYPE_CONFIG && opt->is_set) {
			cfg_fpath = opt->data.str;
			break;
		}
	}

	ret = nereon_parse_noc_options(cfg_fpath, &noc_opts);
	if (ret != 0) {
		DEBUG_PRINT("Failed to parse NOC configuration %s(err:%s)\n", cfg_fpath, nereon_get_err());
		return ret;
	}

	ctx->noc_opts = (void *)noc_opts;

	return 0;
}


/*
 * Print command line usage message
 */

void nereon_print_usage(nereon_ctx_t *ctx)
{
	struct nereon_nos_option *nos_opts = (struct nereon_nos_option *)ctx->nos_opts;

	nereon_cli_print_usage(nos_opts, ctx->nos_opts_count);
}

/*
 * Get the last error message of library
 */

const char *nereon_get_errmsg()
{
	return nereon_get_err();
}

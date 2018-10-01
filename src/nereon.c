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

#include "ctx.h"
#include "nereon.h"

/*
 * Intiialize libnereon context object
 */

nereon_ctx_t *nereon_ctx_init(const char *nos_cfg)
{
	nereon_ctx_priv_t *ctx;

	ctx = nereon_ctx_priv_init(nos_cfg);
	if (!ctx)
		return NULL;

	return (nereon_ctx_t *)ctx;
}

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *ctx)
{
	nereon_ctx_priv_finalize((nereon_ctx_priv_t *)ctx);
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
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *ctx, int argc, char **argv, bool *require_exit)
{
	return nereon_ctx_parse_cmdline((nereon_ctx_priv_t *)ctx, argc, argv, require_exit);
}

/*
 * get libnereon error message
 */

const char *nereon_get_errmsg(void)
{
	return NULL;
}

#if 0

/*
 * parse nereon configuration options
 */

int nereon_get_config_options_t(nereon_ctx_t *ctx, nereon_config_option_t *cfg_opts, int opts_count)
{
	nereon_nos_option_t *nos_opts = (nereon_nos_option_t *)ctx->nos_opts;
	struct nereon_noc_option *noc_opts = (struct nereon_noc_option *)ctx->noc_opts;

	int i;

	for (i = 0; i < opts_count; i++) {
		nereon_config_option_t *cfg_opt = &cfg_opts[i];

		nereon_nos_option_t *nos_opt = NULL;
		struct nereon_noc_option *noc_opt = NULL;

		union nereon_config_data *cfg_data = NULL;

		/* get config from NOS if NOS configuration is exist */
		if (nos_opts) {
			nos_opt = nereon_get_nos_by_name(nos_opts, ctx->nos_opts_count, cfg_opt->name);
			if (!nos_opt) {
				nereon_set_err("Could not get NOS option for config '%s'", cfg_opt->name);
				DEBUG_PRINT("Could not get NOS option for config '%s'\n", cfg_opt->name);
				return -1;
			}

			if (nos_opt->is_cli_set) {
				DEBUG_PRINT("NOS option was set for config '%s'\n", cfg_opt->name);

				if (cfg_opt->is_cli_set)
					*cfg_opt->is_cli_set = true;

				/* set config value from command line at first */
				cfg_data = &nos_opt->data;
			} else {
				/* set config value from configuration file */
				if (strlen(nos_opt->noc_key) > 0 &&
					(noc_opt = nereon_get_noc_option(noc_opts, nos_opt->noc_key))) {
					cfg_data = &noc_opt->data;
				} else if (nos_opt->exist_default) {
					DEBUG_PRINT("Setting NOS default data for config '%s'\n", cfg_opt->name);
					cfg_data = &nos_opt->default_data;
				}
			}
		} else {
			noc_opt = nereon_get_noc_option(noc_opts, cfg_opt->name);
			if (noc_opt)
				cfg_data = &noc_opt->data;
		}

		if (!cfg_data && cfg_opt->type != NEREON_TYPE_ARRAY) {
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
		case NEREON_TYPE_IPPORT:
		case NEREON_TYPE_CONFIG:
			*(char **)(cfg_opt->data) = cfg_data->str;
			break;

		case NEREON_TYPE_ARRAY:
			*((nereon_noc_option_t **)cfg_opt->data) = (void *)noc_opt->parent;
			break;

		default:
			break;
		}
	}

	return 0;
}

/*
 * parse configuration options from object
 */

int nereon_get_noc_configs_t(nereon_noc_option_t *noc_opt, nereon_config_option_t *cfg_opts, int opts_count)
{
	int i;

	DEBUG_PRINT("Getting configuration options from NOC config from '%p'\n", noc_opt);

	for (i = 0; i < opts_count; i++) {
		nereon_config_option_t *cfg_opt = &cfg_opts[i];

		struct nereon_noc_option *child_opt;
		union nereon_config_data *cfg_data = NULL;

		/* get child option by key */
		child_opt = nereon_get_child_noc_option(noc_opt, cfg_opt->name);
		if (child_opt)
			cfg_data = &child_opt->data;

		if (!cfg_data && cfg_opt->type != NEREON_TYPE_ARRAY) {
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
		case NEREON_TYPE_IPPORT:
		case NEREON_TYPE_CONFIG:
			*(char **)(cfg_opt->data) = cfg_data->str;
			break;

		case NEREON_TYPE_KEY:
			*(char **)(cfg_opt->data) = noc_opt->key;
			break;

		case NEREON_TYPE_ARRAY:
			*((nereon_noc_option_t **)cfg_opt->data) = (void *)noc_opt->parent;
			break;

		case NEREON_TYPE_OBJECT:
			*((nereon_noc_option_t **)cfg_opt->data) = (void *)child_opt;
			break;

		default:
			break;
		}
	}

	return 0;
}

/*
 * parse NOC configuration file
 */

int nereon_parse_config_file(nereon_ctx_t *ctx, const char *noc_cfg)
{
	int i, ret;

	const char *cfg_fpath = noc_cfg;

	struct nereon_noc_option *noc_opts = NULL;

	/* check if configuration file was specified in command line */
	for (i = 0; i < ctx->nos_opts_count; i++) {
		nereon_nos_option_t *opt = &ctx->nos_opts[i];

		if (opt->type == NEREON_TYPE_CONFIG && opt->is_cli_set) {
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
	nereon_cli_print_usage(ctx->nos_opts, ctx->nos_opts_count);
}

/*
 * Get the last error message of library
 */

const char *nereon_get_errmsg()
{
	return nereon_get_err();
}

#endif

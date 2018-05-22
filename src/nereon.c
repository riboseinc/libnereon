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

#include "common.h"
#include "err.h"
#include "util.h"
#include "nereon.h"

#include "meta.h"
#include "cmdline.h"
#include "cfg.h"

/*
 * Intiialize libnereon context object
 */

int nereon_ctx_init(nereon_ctx_t *ctx, const char *prog_cfg_fpath, const char *meta_cfg_fpath)
{
	struct nereon_meta_options *meta_opts = NULL;
	int meta_opts_count = 0;

	struct nereon_cfg_options *cfg_opts = NULL;

	DEBUG_PRINT("Initializing libnereon context with prog_cfg:%s and meta_cfg:%s\n",
			prog_cfg_fpath, meta_cfg_fpath);

	memset(ctx, 0, sizeof(nereon_ctx_t));

	/* parse meta options */
	if (meta_cfg_fpath && nereon_parse_meta_options(meta_cfg_fpath, &meta_opts, &meta_opts_count) != 0) {
		DEBUG_PRINT("Failed to parse meta options(err:%s)\n", nereon_get_err());
		return -1;
	}

	/* parse configuration options */
	if (prog_cfg_fpath && nereon_parse_cfg_options(prog_cfg_fpath, &cfg_opts) != 0) {
		DEBUG_PRINT("Faild to parse configuration options(err:%s)\n", nereon_get_err());
		nereon_ctx_finalize(ctx);
		return -1;
	}

	ctx->meta_opts = (void *)meta_opts;
	ctx->meta_opts_count = meta_opts_count;

	ctx->cfg_opts = cfg_opts;

	return 0;
}

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *ctx)
{
	struct nereon_meta_options *meta_opts = (struct nereon_meta_options *)ctx->meta_opts;
	struct nereon_cfg_options *cfg_opts = (struct nereon_cfg_options *)ctx->cfg_opts;

	nereon_free_cfg_options(cfg_opts);
	nereon_free_meta_options(meta_opts, ctx->meta_opts_count);
}

/*
 * Parse configuration file
 */

int nereon_parse_config(nereon_ctx_t *ctx, const char *cfg_fpath)
{
	struct nereon_cfg_options *cfg_opt = NULL;
	char *cfg_hcl = NULL;

	int ret;

	DEBUG_PRINT("Parsing configuration file '%s'\n", cfg_fpath);

	/* get configuration contents */
	if (read_file_contents(cfg_fpath, &cfg_hcl) == 0) {
		nereon_set_err("Could not read configuration file '%s'", cfg_fpath);
		return -1;
	}

	ret = nereon_parse_cfg_options(cfg_hcl, &cfg_opt);
	if (ret == 0) {
		ctx->cfg_opts = (void *)cfg_opt;
	}
	free(cfg_hcl);

	return ret;
}

/*
 * get configuration option
 */

int nereon_get_config_option(nereon_ctx_t *ctx, const char *key, void *val)
{
	/* get configuration */

	return 0;
}

/*
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *ctx, int argc, char **argv)
{
	struct nereon_meta_options *meta_opts = (struct nereon_meta_options *)ctx->meta_opts;

	return nereon_cli_parse(meta_opts, ctx->meta_opts_count, argc, argv);
}

/*
 * Print command line usage message
 */

void nereon_print_usage(nereon_ctx_t *ctx)
{
	struct nereon_meta_options *meta_opts = (struct nereon_meta_options *)ctx->meta_opts;

	nereon_cli_print_usage(meta_opts, ctx->meta_opts_count);
}

/*
 * Get the last error message of library
 */

const char *nereon_get_errmsg()
{
	return nereon_get_err();
}

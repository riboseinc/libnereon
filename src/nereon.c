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

#include "meta.h"
#include "cmdline.h"
#include "err.h"
#include "util.h"
#include "cfg.h"

#include "nereon.h"

/*
 * Intiialize libnereon context object
 */

int nereon_ctx_init(nereon_ctx_t *mctx, const char *prog_cfg_fpath, const char *meta_cfg_fpath)
{
	struct mcfg_meta_options *meta_opts = NULL;
	int meta_opts_count = 0;

	struct mcfg_cfg_options *cfg_opts = NULL;

	DEBUG_PRINT("Initializing libnereon context with prog_cfg:%s and meta_cfg:%s\n",
			prog_cfg_fpath, meta_cfg_fpath);

	memset(mctx, 0, sizeof(nereon_ctx_t));

	/* parse meta options */
	if (meta_cfg_fpath && mcfg_parse_meta_options(meta_cfg_fpath, &meta_opts, &meta_opts_count) != 0) {
		DEBUG_PRINT("Failed to parse meta options(err:%s)\n", mcfg_get_err());
		return -1;
	}

	/* parse configuration options */
	if (prog_cfg_fpath && mcfg_parse_cfg_options(prog_cfg_fpath, &cfg_opts) != 0) {
		DEBUG_PRINT("Faild to parse configuration options(err:%s)\n", mcfg_get_err());
		nereon_ctx_finalize(mctx);
		return -1;
	}

	/* merge config with metaconfig */
	if (cfg_opts && mcfg_merge_cfg_options(cfg_opts, meta_opts, meta_opts_count) != 0) {
		DEBUG_PRINT("Failed to merge config with metaconfig(err:%s)\n", mcfg_get_err());
		nereon_ctx_finalize(mctx);
		return -1;
	}

	mctx->meta_opts = (void *)meta_opts;
	mctx->meta_opts_count = meta_opts_count;

	mctx->cfg_opts = cfg_opts;

	return 0;
}

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *mctx)
{
	struct mcfg_meta_options *meta_opts = (struct mcfg_meta_options *)mctx->meta_opts;
	struct mcfg_cfg_options *cfg_opts = (struct mcfg_cfg_options *)mctx->cfg_opts;

	mcfg_free_cfg_options(cfg_opts);
	mcfg_free_meta_options(meta_opts, mctx->meta_opts_count);
}

/*
 * Parse configuration file
 */

int nereon_parse_config(nereon_ctx_t *mctx, const char *cfg_fpath)
{
	struct mcfg_cfg_options *cfg_opt = NULL;
	char *cfg_hcl = NULL;

	int ret;

	DEBUG_PRINT("Parsing configuration file '%s'\n", cfg_fpath);

	/* get configuration contents */
	if (read_file_contents(cfg_fpath, &cfg_hcl) == 0) {
		mcfg_set_err("Could not read configuration file '%s'", cfg_fpath);
		return -1;
	}

	ret = mcfg_parse_cfg_options(cfg_hcl, &cfg_opt);
	if (ret == 0) {
		mctx->cfg_opts = (void *)cfg_opt;
	}
	free(cfg_hcl);

	return ret;
}

/*
 * get configuration option
 */

void nereon_get_config_option(nereon_ctx_t *mctx, const char *opt_key, void **opt_val)
{
	
}

/*
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *mctx, int argc, char **argv)
{
	struct mcfg_meta_options *meta_opts = (struct mcfg_meta_options *)mctx->meta_opts;

	return mcfg_parse_cmdline(meta_opts, mctx->meta_opts_count, argc, argv);
}

/*
 * Print command line usage message
 */

void nereon_print_usage(nereon_ctx_t *mctx)
{
	struct mcfg_meta_options *meta_opts = (struct mcfg_meta_options *)mctx->meta_opts;

	mcfg_print_cmdline_usage(meta_opts, mctx->meta_opts_count);
}

/*
 * Get the last error message of library
 */

const char *nereon_get_errmsg()
{
	return mcfg_get_err();
}

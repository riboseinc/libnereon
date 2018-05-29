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

#include "nos.h"
#include "cli.h"
#include "noc.h"

/*
 * Intiialize libnereon context object
 */

int nereon_ctx_init(nereon_ctx_t *ctx, const char *nos_cfg)
{
	struct nereon_nos_options *nos_opts = NULL;
	int nos_opts_count = 0;

	struct nereon_cfg_options *cfg_opts = NULL;

	DEBUG_PRINT("Initializing libnereon context");

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
	struct nereon_nos_options *nos_opts = (struct nereon_nos_options *)ctx->nos_opts;
	struct nereon_noc_options *noc_opts = (struct nereon_noc_options *)ctx->noc_opts;

	nereon_free_noc_options(noc_opts);
	nereon_free_nos_options(nos_opts, ctx->nos_opts_count);
}

/*
 * get configuration option
 */

int nereon_get_config_option(nereon_ctx_t *ctx, const char *key, void *val)
{
	return 0;
}

/*
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *ctx, int argc, char **argv)
{
	struct nereon_nos_options *nos_opts = (struct nereon_nos_options *)ctx->nos_opts;

	return nereon_cli_parse(nos_opts, ctx->nos_opts_count, argc, argv);
}

/*
 * Print command line usage message
 */

void nereon_print_usage(nereon_ctx_t *ctx)
{
	struct nereon_nos_options *nos_opts = (struct nereon_nos_options *)ctx->nos_opts;

	nereon_cli_print_usage(nos_opts, ctx->nos_opts_count);
}

/*
 * Get the last error message of library
 */

const char *nereon_get_errmsg()
{
	return nereon_get_err();
}

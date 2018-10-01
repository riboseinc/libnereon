#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ctx.h"

/*
 * initialize libnereon context
 */

nereon_ctx_priv_t *nereon_ctx_priv_init(const char *nos_cfg)
{
	nereon_ctx_priv_t *ctx;

	/* allocate memory for libnereon context */
	ctx = (nereon_ctx_priv_t *)malloc(sizeof(nereon_ctx_priv_t));
	if (!ctx)
		return NULL;

	memset(ctx, 0, sizeof(nereon_ctx_priv_t));

	/* parse NOS schema */
	if (nereon_parse_nos_schema(nos_cfg, &ctx->nos_schema) != 0) {
		free(ctx);
		return NULL;
	}

	return ctx;
}

/*
 * finalize libnereon context
 */

void nereon_ctx_priv_finalize(nereon_ctx_priv_t *ctx)
{
	/* free NOS schema */
	nereon_free_nos_schema(&ctx->nos_schema);

	/* free context */
	free(ctx);
}

/*
 * parse command line
 */

int nereon_ctx_parse_cmdline(nereon_ctx_priv_t *ctx, int argc, char **argv, bool *required_exit)
{
	return nereon_cli_parse(ctx, argc, argv, required_exit);
}

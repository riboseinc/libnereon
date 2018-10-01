#ifndef __NEREON_CTX_H__
#define __NEREON_CTX_H__

struct nereon_ctx_priv;

#include "common.h"
#include "nos.h"
#include "cli.h"

/*
 * libnereon context object
 */

typedef struct nereon_ctx_priv {
	nereon_nos_schema_t nos_schema;
	nereon_cli_option_t root_cli_opt;
} nereon_ctx_priv_t;

/*
 * initialize libnereon context
 */

nereon_ctx_priv_t *nereon_ctx_priv_init(const char *nos_cfg);

/*
 * finalize libnereon context
 */

void nereon_ctx_priv_finalize(nereon_ctx_priv_t *ctx);

/*
 * parse command line
 */

int nereon_ctx_parse_cmdline(nereon_ctx_priv_t *ctx, int argc, char **argv, bool *required_exit);

#endif /* __NEREON_CTX_H__ */

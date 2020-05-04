#ifndef __NEREON_CTX_H__
#define __NEREON_CTX_H__

struct nereon_ctx_priv;

#include <ucl.h>

#include "common.h"
#include "err.h"
#include "util.h"
#include "nos.h"
#include "cli.h"

/*
 * libnereon context object
 */

typedef struct nereon_ctx_priv {
	nereon_nos_schema_t nos_schema;
} nereon_ctx_priv_t;

/*
 * initialize libnereon context
 */

nereon_ctx_priv_t *ctx_priv_init(const char *nos_cfg);

/*
 * finalize libnereon context
 */

void ctx_priv_finalize(nereon_ctx_priv_t *ctx);

/*
 * parse command line
 */

int ctx_parse_cmdline(nereon_ctx_priv_t *ctx, int argc, char **argv, bool *required_exit);

/*
 * print CLI usage
 */

void ctx_print_usage(nereon_ctx_priv_t *ctx);

#endif /* __NEREON_CTX_H__ */

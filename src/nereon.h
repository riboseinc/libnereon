#ifndef __NEREON_H__
#define __NEREON_H__

#include <stdbool.h>

#include "common.h"
#include "nos.h"
#include "cli.h"
#include "noc.h"

#define STRINGIZE(x)               #x
#define STRINGIZE_VALUE_OF(x)      STRINGIZE(x)

typedef void *nereon_ctx_t;

/*
 * Initialize libnereon context object
 */

nereon_ctx_t *nereon_ctx_init(const char *nos_cfg);

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *ctx);

/*
 * get libnereon error message
 */

const char *nereon_get_errmsg(void);

#if 0

/*
 * get libnereon version
 */

const char *nereon_get_version_info(nereon_ctx_t *ctx);

/*
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *ctx, int argc, char **argv, bool *require_exit);

/*
 * Parse NOC configuration file
 */

int nereon_parse_config_file(nereon_ctx_t *ctx, const char *noc_cfg);

/*
 * Print libnereon command line usage
 */

void nereon_print_usage(nereon_ctx_t *ctx);

/*
 * Get the last error message
 */

const char *nereon_get_errmsg(void);

#endif

#endif /* __NEREON_H__ */

#ifndef __NEREON_H__
#define __NEREON_H__

#include <stdbool.h>

#include "common.h"
#include "nos.h"
#include "cli.h"
#include "noc.h"

#define STRINGIZE(x)               #x
#define STRINGIZE_VALUE_OF(x)      STRINGIZE(x)

/*
 * libnereon configuration option
 */

typedef struct nereon_config_option {
	const char *name;
	enum NEREON_CONFIG_TYPE type;

	bool mandatory;
	bool *is_cli_set;

	void *data;
} nereon_config_option_t;

/*
 * libnereon context structure
 */

typedef struct nereon_ctx {
	nereon_nos_option_t *nos_opts;
	int nos_opts_count;

	nereon_noc_option_t *noc_opts;

	bool use_nos_cfg;
} nereon_ctx_t;

/*
 * Initialize libnereon context object
 */

int nereon_ctx_init(nereon_ctx_t *ctx, const char *nos_cfg);

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *ctx);

/*
 * get libnereon version
 */

const char *nereon_get_version_info(void);

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
 * parse configuration options from context
 */

int nereon_get_config_options_t(nereon_ctx_t *ctx, nereon_config_option_t *cfg_opts, int opts_count);

#define nereon_get_config_options(ctx, cfg_opts) \
	nereon_get_config_options_t(ctx, cfg_opts, sizeof(cfg_opts) / sizeof(struct nereon_config_option))

/*
 * parse configuration options from object
 */

int nereon_get_noc_configs_t(nereon_noc_option_t *noc_opt, nereon_config_option_t *cfg_opts, int opts_count);

#define nereon_get_noc_configs(obj, cfg_opts) \
	nereon_get_noc_configs_t(obj, cfg_opts, sizeof(cfg_opts) / sizeof(struct nereon_config_option))

/*
 * interate nereon object
 */

#define nereon_object_object_foreach(parent_obj, val) \
	struct nereon_noc_option *noc_parent_opt = (struct nereon_noc_option *)parent_obj; \
	struct nereon_noc_option *val; \
	for (val = noc_parent_opt->childs; val != NULL; val = val->next)

/*
 * Get the last error message
 */

const char *nereon_get_errmsg(void);

#endif /* __NEREON_H__ */

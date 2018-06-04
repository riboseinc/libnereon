#ifndef __NEREON_H__
#define __NEREON_H__

#include <stdbool.h>

/*
 * libnereon configuration types
 */

enum NEREON_CONFIG_TYPE {
	NEREON_TYPE_INT = 0,
	NEREON_TYPE_BOOL,
	NEREON_TYPE_STRING,
	NEREON_TYPE_ARRAY,
	NEREON_TYPE_IPPORT,
	NEREON_TYPE_FLOAT,
	NEREON_TYPE_OBJECT,
	NEREON_TYPE_CONFIG,
	NEREON_TYPE_HELPER,
	NEREON_TYPE_UNKNOWN
};

/*
 * libnereon configuration data
 */

union nereon_config_data {
	int i;
	char *str;
	bool b;
	double d;
};

/*
 * libnereon configuration option
 */

typedef struct nereon_config_option {
	enum NEREON_CONFIG_TYPE type;
	union nereon_config_data data;
} nereon_config_option_t;

/*
 * libnereon context structure
 */

typedef struct nereon_ctx {
	void *nos_opts;
	int nos_opts_count;

	void *noc_opts;
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
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *ctx, int argc, char **argv);

/*
 * Print libnereon command line usage
 */

void nereon_print_usage(nereon_ctx_t *ctx);

/*
 * parse configuration options
 */

int nereon_get_config_options(nereon_ctx_t *ctx, nereon_config_option_t *cfg_opts, int opts_count);

/*
 * Get the last error message
 */

const char *nereon_get_errmsg(void);

#endif /* __NEREON_H__ */

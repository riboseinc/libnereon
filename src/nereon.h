#ifndef __NEREON_H__
#define __NEREON_H__

/*
 * libnereon context structure
 */

typedef struct _ctx {
	void *meta_opts;
	int meta_opts_count;

	void *cfg_opts;
} nereon_ctx_t;

/*
 * Initialize libnereon context object
 */

int nereon_ctx_init(nereon_ctx_t *mctx, const char *prog_cfg_fpath, const char *meta_cfg_fpath);

/*
 * Finalize libnereon context object
 */

void nereon_ctx_finalize(nereon_ctx_t *mctx);

/*
 * Parse command line arguments
 */

int nereon_parse_cmdline(nereon_ctx_t *mctx, int argc, char **argv);

/*
 * Print libnereon command line usage
 */

void nereon_print_usage(nereon_ctx_t *mctx);

/*
 * get configuration option
 */

void nereon_get_config_option(nereon_ctx_t *mctx, const char *opt_key, void **opt_val);

/*
 * Get the last error message
 */

const char *nereon_get_errmsg(void);

#endif /* __NEREON_H__ */

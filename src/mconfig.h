#ifndef __MCONFIG_H__
#define __MCONFIG_H__

/*
 * multiconfig context structure
 */

typedef struct mconfig_ctx {
	void *hcl_opts;
	int hcl_opts_count;

	void *cfg_opts;
} mconfig_ctx_t;

/*
 * Initialize multiconfig context object
 */

int mconfig_ctx_init(mconfig_ctx_t *mctx, const char *hcl_options);

/*
 * Finalize multiconfig context object
 */

void mconfig_ctx_finalize(mconfig_ctx_t *mctx);

/*
 * Parse command line arguments
 */

int mconfig_parse_cmdline(mconfig_ctx_t *mctx, int argc, char **argv);

/*
 * Parse configuration file
 */

int mconfig_parse_config(mconfig_ctx_t *mctx, const char *cfg_fpath);

/*
 * Print multiconfig command line usage
 */

void mconfig_print_usage(mconfig_ctx_t *mctx);

/*
 * Get the last error message
 */

const char *mconfig_get_errmsg(void);

#endif /* __MCONFIG_H__ */

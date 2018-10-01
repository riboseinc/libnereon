#ifndef __NEREON_CMDLINE_H__
#define __NEREON_CMDLINE_H__

typedef struct nereon_cli_option {
	nereon_nos_option_t *nos_opt;

	char **args;                       /* followed arguments or sub options */
	int args_count;

	struct nereon_cli_option *next;
} nereon_cli_option_t;

/*
 * parse command line arguments based on NOS schema
 */

int nereon_cli_parse(struct nereon_ctx_priv *ctx, int argc, char **argv, bool *require_exit);

/*
 * free cli options
 */

void nereon_cli_free(nereon_cli_option_t *root_opt);

#if 0

/*
 * print command line usage
 */

void nereon_cli_print_usage(struct nereon_nos_option *nos_opts, int nos_opts_count);

#endif

#endif /* __NEREON_CMDLINE_H__ */

#ifndef __NEREON_CMDLINE_H__
#define __NEREON_CMDLINE_H__

/*
 * parse command line arguments based on NOS schema
 */

int cli_parse_argv(struct nereon_ctx_priv *ctx, int argc, char **argv, bool *require_exit);

/*
 * print CLI usage message
 */

void cli_print_usage(struct nereon_ctx_priv *ctx);

#if 0

/*
 * free cli options
 */

void nereon_cli_free(nereon_cli_option_t *root_opt);

/*
 * print command line usage
 */

void nereon_cli_print_usage(struct nereon_nos_option *nos_opts, int nos_opts_count);

#endif

#endif /* __NEREON_CMDLINE_H__ */

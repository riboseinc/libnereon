#ifndef __NEREON_CMDLINE_H__
#define __NEREON_CMDLINE_H__

/*
 * parse command line
 */

int nereon_cli_parse(struct nereon_meta_options *meta_opts, int meta_opts_count, int argc, char **argv);

/*
 * print command line usage
 */

void nereon_cli_print_usage(struct nereon_meta_options *meta_opts, int meta_opts_count);

#endif /* __NEREON_CMDLINE_H__ */

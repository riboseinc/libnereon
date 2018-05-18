#ifndef __MCONFIG_CMDLINE_H__
#define __MCONFIG_CMDLINE_H__

/*
 * parse command line
 */

int mcfg_parse_cmdline(struct mcfg_meta_options *meta_opts, int meta_opts_count, int argc, char **argv);

/*
 * print command line usage
 */

void mcfg_print_cmdline_usage(struct mcfg_meta_options *meta_opts, int meta_opts_count);

#endif /* __MCONFIG_CMDLINE_H__ */

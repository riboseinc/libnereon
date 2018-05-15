#ifndef __CMDLINE_H__
#define __CMDLINE_H__

/*
 * parse command line
 */

int mcfg_parse_cmdline(struct mcfg_hcl_options *hcl_opts, int hcl_opts_count, int argc, char **argv);

/*
 * print command line usage
 */

void mcfg_print_cmdline_usage(struct mcfg_hcl_options *hcl_opts, int hcl_opts_count);

#endif /* __CMDLINE_H__ */

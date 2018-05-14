#ifndef __MCONFIG_H__
#define __MCONFIG_H__

/*
 * multiconfig export functions
 */

int mconfig_init(const char *hcl_options, int argc, char **argv);

const char *mconfig_get_err(void);

#endif /* __MCONFIG_H__ */

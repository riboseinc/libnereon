#ifndef __MCONFIG_CFG_H__
#define __MCONFIG_CFG_H__

/*
 * multiconfig config HCL options
 */

struct mcfg_cfg_options {
	char cfg_key[CFG_MAX_KEY_NAME];
	enum MCFG_TYPE cfg_type;

	struct mcfg_cfg_options *childs;
	struct mcfg_cfg_options *next;

	union _cfg_data cfg_data;
};

/*
 * parse configuration options
 */

int mcfg_parse_cfg_options(const char *cfg_fpath, struct mcfg_cfg_options **cfg_opts);

/*
 * free configuration options
 */

void mcfg_free_cfg_options(struct mcfg_cfg_options *cfg_opts);

#endif /* __MCONFIG_CFG_H__ */

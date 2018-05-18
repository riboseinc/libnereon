#ifndef __MCONFIG_HCL_H__
#define __MCONFIG_HCL_H__

/*
 * multiconfig HCL options
 */

struct mcfg_meta_options {
	char cfg_name[CFG_MAX_NAME];
	enum MCFG_TYPE cfg_type;

	bool helper;

	char sw_short[2];
	char sw_long[CFG_MAX_LONG_SWITCH];

	char desc_short[CFG_MAX_SHORT_DESC];
	char desc_long[CFG_MAX_LONG_DESC];

	char cfg_env[CFG_MAX_ENV_NAME];
	char cfg_key[CFG_MAX_KEY_NAME];

	union _cfg_data cfg_data;
};

/*
 * parse multiconfig HCL options
 */

int mcfg_parse_meta_options(const char *meta_cfg_fpath, struct mcfg_meta_options **meta_opts, int *meta_opts_count);

/*
 * free HCL options
 */

void mcfg_free_meta_options(struct mcfg_meta_options *meta_opts, int meta_opts_count);

#endif /* __MCONFIG_HCL_H__ */

#ifndef __NEREON_META_H__
#define __NEREON_META_H__

/*
 * multiconfig HCL options
 */

struct nereon_meta_options {
	char cfg_name[CFG_MAX_NAME];
	enum NEREON_TYPE cfg_type;

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

int nereon_parse_meta_options(const char *meta_cfg_fpath, struct nereon_meta_options **meta_opts, int *meta_opts_count);

/*
 * free HCL options
 */

void nereon_free_meta_options(struct nereon_meta_options *meta_opts, int meta_opts_count);

#endif /* __NEREON_META_H__ */

#ifndef __NEREON_CFG_H__
#define __NEREON_CFG_H__

/*
 * multiconfig config HCL options
 */

struct nereon_cfg_options {
	char cfg_key[CFG_MAX_KEY_NAME];
	enum NEREON_TYPE cfg_type;

	struct nereon_cfg_options *childs;
	struct nereon_cfg_options *next;

	union _cfg_data cfg_data;
};

/*
 * parse configuration options
 */

int nereon_parse_cfg_options(const char *cfg_fpath, struct nereon_cfg_options **cfg_opts);

/*
 * free configuration options
 */

void nereon_free_cfg_options(struct nereon_cfg_options *cfg_opts);

/*
 * merge configuration options
 */

int nereon_merge_cfg_options(struct nereon_cfg_options *cfg_opts, struct nereon_meta_options *meta_opts, int meta_opts_count);

#endif /* __NEREON_CFG_H__ */

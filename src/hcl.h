#ifndef __MCONFIG_HCL_H__
#define __MCONFIG_HCL_H__

/*
 * multiconfig configuration types
 */

enum MCFG_TYPE {
	MCFG_TYPE_INT = 0,
	MCFG_TYPE_BOOL,
	MCFG_TYPE_STRING,
	MCFG_TYPE_ARRAY,
	MCFG_TYPE_IPPORT,
	MCFG_TYPE_UNKNOWN
};

union _cfg_data {
	int i;
	char *str;
	bool b;
};

/*
 * multiconfig HCL options
 */

struct mcfg_hcl_options {
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
 * multiconfig config HCL options
 */

struct mcfg_cfg_options {
	char cfg_key[CFG_MAX_KEY_NAME];
	enum MCFG_TYPE cfg_type;

	int childs_count;
	struct mcfg_cfg_options *childs;

	struct mcfg_cfg_options *next;

	union _cfg_data cfg_data;
};

/*
 * parse multiconfig HCL options
 */

int mcfg_parse_hcl_options(const char *hcl, struct mcfg_hcl_options **hcl_opts, int *hcl_opts_count);

/*
 * free HCL options
 */

void mcfg_free_hcl_options(struct mcfg_hcl_options *hcl_opts, int hcl_opts_count);

#endif /* __MCONFIG_HCL_H__ */

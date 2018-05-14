#ifndef __MCONFIG_HCL_H__
#define __MCONFIG_HCL_H__

/* configuration types */
enum CFG_TYPE {
	CFG_TYPE_INT = 0,
	CFG_TYPE_BOOL,
	CFG_TYPE_STRING,
	CFG_TYPE_ARRAY,
	CFG_TYPE_IPPORT,
	CFG_TYPE_UNKNOWN
};

struct mconfig_hcl_options {
	char cfg_name[CFG_MAX_NAME];
	enum CFG_TYPE cfg_type;

	bool helper;

	char sw_short[2];
	char sw_long[CFG_MAX_LONG_SWITCH];

	char desc_short[CFG_MAX_SHORT_DESC];
	char desc_long[CFG_MAX_LONG_DESC];

	char cfg_env[CFG_MAX_ENV_NAME];
	char cfg_key[CFG_MAX_KEY_NAME];

	union {
		int i;
		char *str;
		bool b;
	} cfg_data;
};

/* parse HCL options */
int mconfig_parse_hcl_options(const char *hcl, struct mconfig_hcl_options **hcl_opts, int *hcl_opts_count);

#endif /* __MCONFIG_HCL_H__ */

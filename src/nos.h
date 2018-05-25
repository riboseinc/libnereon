#ifndef __NEREON_NOS_H__
#define __NEREON_NOS_H__

/*
 * libnereon NOS options
 */

struct nereon_nos_options {
	char name[CFG_MAX_NAME];
	enum NEREON_CONFIG_TYPE type;

	bool is_set;

	char sw_short[2];
	char sw_long[CFG_MAX_LONG_SWITCH];

	char desc_short[CFG_MAX_SHORT_DESC];
	char desc_long[CFG_MAX_LONG_DESC];

	char env[CFG_MAX_ENV_NAME];
	char noc_key[CFG_MAX_KEY_NAME];

	union nereon_config_data data;
};

/*
 * parse multiconfig NOS options
 */

int nereon_parse_nos_options(const char *nos_cfg, struct nereon_nos_options **nos_opts, int *nos_opts_count);

/*
 * free NOS options
 */

void nereon_free_nos_options(struct nereon_nos_options *nos_opts, int nos_opts_count);

/*
 * get NOS option
 */

int nereon_get_nos_option(struct nereon_nos_options *nos_opts, int nos_opts_count, const char *key, nereon_config_option_t *value);

#endif /* __NEREON_NOS_H__ */

#ifndef __NEREON_NOS_H__
#define __NEREON_NOS_H__

/*
 * libnereon NOS options
 */

typedef struct nereon_nos_option {
	char name[CFG_MAX_NAME];
	enum NEREON_CONFIG_TYPE type;

	bool is_cli_set;

	char sw_short[CFG_MAX_SHORT_SWITCH];
	char sw_long[CFG_MAX_LONG_SWITCH];

	char desc_short[CFG_MAX_SHORT_DESC];
	char desc_long[CFG_MAX_LONG_DESC];

	int cli_args_count;
	struct nereon_nos_option *cli_args;

	bool exist_cli_default;
	union nereon_config_data cli_default_data;

	char env[CFG_MAX_ENV_NAME];
	char noc_key[CFG_MAX_KEY_NAME];

	union nereon_config_data data;

	bool exist_default;
	union nereon_config_data default_data;
} nereon_nos_option_t;

/*
 * parse multiconfig NOS options
 */

int nereon_parse_nos_options(const char *nos_cfg, nereon_nos_option_t **nos_opts, int *nos_opts_count);

/*
 * free NOS options
 */

void nereon_free_nos_options(nereon_nos_option_t *nos_opts, int nos_opts_count);

/*
 * get NOS option by switch
 */

nereon_nos_option_t *nereon_get_nos_by_switch(nereon_nos_option_t *nos_opts, int nos_opts_count,
		const char *sw_key);

/*
 * get NOS option by name
 */

nereon_nos_option_t *nereon_get_nos_by_name(nereon_nos_option_t *nos_opts, int nos_opts_count,
		const char *name);

#endif /* __NEREON_NOS_H__ */

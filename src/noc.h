#ifndef __NEREON_NOC_H__
#define __NEREON_NOC_H__

/*
 * libnereon NOC options
 */

typedef struct nereon_noc_option {
	char key[NR_MAX_KEY_NAME];
	enum NEREON_OPT_TYPE type;

	struct nereon_noc_option *childs;
	struct nereon_noc_option *next;
	struct nereon_noc_option *parent;

	union nereon_config_data data;
} nereon_noc_option_t;

/*
 * parse NOC options
 */

int nereon_parse_noc_options(const char *cfg_fpath, nereon_noc_option_t **noc_opts);

/*
 * free NOC options
 */

void nereon_free_noc_options(nereon_noc_option_t *noc_opts);

/*
 * get NOC option
 */

nereon_noc_option_t *nereon_get_noc_option(nereon_noc_option_t *noc_opts, const char *cfg_key);

/*
 * get NOC option from childs
 */

nereon_noc_option_t *nereon_get_child_noc_option(nereon_noc_option_t *noc_opt, const char *cfg_key);

#endif /* __NEREON_NOC_H__ */

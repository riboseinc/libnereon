#ifndef __NEREON_NOC_H__
#define __NEREON_NOC_H__

/*
 * libnereon NOC options
 */

struct nereon_noc_option {
	char key[CFG_MAX_KEY_NAME];
	enum NEREON_CONFIG_TYPE type;

	struct nereon_noc_option *childs;
	struct nereon_noc_option *next;
	struct nereon_noc_option *parent;

	union nereon_config_data data;
};

/*
 * parse NOC options
 */

int nereon_parse_noc_options(const char *cfg_fpath, struct nereon_noc_option **noc_opts);

/*
 * free NOC options
 */

void nereon_free_noc_options(struct nereon_noc_option *noc_opts);

/*
 * get NOC option
 */

struct nereon_noc_option *nereon_get_noc_option(struct nereon_noc_option *noc_opts, const char *cfg_key);

/*
 * get NOC option from childs
 */

struct nereon_noc_option *nereon_get_child_noc_option(struct nereon_noc_option *noc_opt, const char *cfg_key);

#endif /* __NEREON_NOC_H__ */

#ifndef __NEREON_NOC_H__
#define __NEREON_NOC_H__

/*
 * libnereon NOC options
 */

struct nereon_noc_options {
	char key[CFG_MAX_KEY_NAME];
	enum NEREON_CONFIG_TYPE type;

	struct nereon_noc_options *childs;
	struct nereon_noc_options *next;

	union nereon_config_data data;
};

/*
 * parse NOC options
 */

int nereon_parse_noc_options(const char *cfg_fpath, struct nereon_noc_options **noc_opts);

/*
 * free NOC options
 */

void nereon_free_noc_options(struct nereon_noc_options *noc_opts);

#endif /* __NEREON_NOC_H__ */

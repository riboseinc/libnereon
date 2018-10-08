#ifndef __NEREON_NOS_H__
#define __NEREON_NOS_H__

enum NEREON_SUBOPT_TYPE {
	NEREON_SUBOPT_SINGLE = 0,
	NEREON_SUBOPT_MIXED
};

/*
 * libnereon NOS prog info
 */

typedef struct nereon_nos_proginfo {
	char prog_name[NR_MAX_NAME];
	char desc[NR_MAX_DESC];
	char version[NR_MAX_VERSION];
	char copyright[NR_MAX_COPYRIGHT];
	char homepage[NR_MAX_URL];
	char license[NR_MAX_URL];
} nereon_nos_proginfo_t;

/*
 * nereon NOS sub option
 */

typedef struct nereon_nos_subopts {
	bool requires;
	enum NEREON_SUBOPT_TYPE type;

	struct nereon_nos_option **opts;
	int opts_count;
} nereon_nos_subopts_t;

/*
 * nereon NOS option
 */

typedef struct nereon_nos_option {
	char name[NR_MAX_NAME];
	enum NEREON_OPT_TYPE type;

	int is_option;

	char sw[NR_MAX_SWITCH];
	char desc[NR_MAX_DESC];
	char hint[NR_MAX_HINT];

	char default_val[NR_MAX_DEFAULT_VAL];

	nereon_opt_data_t data;

	nereon_nos_subopts_t sub_opts;
	struct nereon_nos_option *parent_opt;
} nereon_nos_option_t;

/*
 * libnereon NOS options
 */

typedef struct nereon_nos_schema {
	nereon_nos_proginfo_t    prog_info;
	nereon_nos_option_t      root_opt;
} nereon_nos_schema_t;

/*
 * parse NOS schema
 */

int nereon_parse_nos_schema(const char *nos_cfg, nereon_nos_schema_t *nos_schema);

/*
 * free NOS schema
 */

void nereon_free_nos_schema(nereon_nos_schema_t *nos_schema);

/*
 * get NOS option by switch
 */

nereon_nos_option_t *nereon_get_nos_by_sw(nereon_nos_option_t *parent_opt, const char *sw);

/*
 * set NOS option
 */

int nereon_set_nos_option(nereon_nos_option_t *nos_opt, int argc, char **argv, int *index, bool *require_exit);

#endif /* __NEREON_NOS_H__ */

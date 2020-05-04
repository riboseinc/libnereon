#ifndef __NEREON_COMMON_H__
#define __NEREON_COMMON_H__

#include <stdbool.h>

/* constant macros */
#define NR_MAX_NAME                64
#define NR_MAX_TYPE                64
#define NR_MAX_SWITCH              64
#define NR_MAX_DESC                256
#define NR_MAX_HINT                128
#define NR_MAX_ENV_NAME            64
#define NR_MAX_KEY_NAME            128
#define NR_MAX_ERR_MSG             2048
#define NR_MAX_VERSION             32
#define NR_MAX_COPYRIGHT           128
#define NR_MAX_URL                 1024
#define NR_MAX_DEFAULT_VAL         512

/*
 * libnereon configuration data
 */

typedef union nereon_opt_data {
	int i;
	char *str;
	bool b;
	double d;

	struct nereon_opt_array {
		char **items;
		int items_count;
	} arr;
} nereon_opt_data_t;

/*
 * libnereon configuration types
 */

enum NEREON_OPT_TYPE {
	NEREON_TYPE_INT = 0,
	NEREON_TYPE_BOOL,
	NEREON_TYPE_STRING,
	NEREON_TYPE_ARRAY,
	NEREON_TYPE_FLOAT,
	NEREON_TYPE_CONFIG,
	NEREON_TYPE_HELPER,
	NEREON_TYPE_VERSION,
	NEREON_TYPE_UNKNOWN
};

#endif /* __NEREON_COMMON_H__ */

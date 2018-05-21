#ifndef __NEREON_COMMON_H__
#define __NEREON_COMMON_H__

#include <stdbool.h>

/* constant macros */
#define CFG_MAX_NAME                      64
#define CFG_MAX_LONG_SWITCH               32
#define CFG_MAX_SHORT_DESC                32
#define CFG_MAX_LONG_DESC                 128
#define CFG_MAX_ENV_NAME                  64
#define CFG_MAX_KEY_NAME                  128
#define CFG_MAX_ERR_MSG                   1024

/*
 * multiconfig configuration types
 */

enum NEREON_TYPE {
	NEREON_TYPE_INT = 0,
	NEREON_TYPE_BOOL,
	NEREON_TYPE_STRING,
	NEREON_TYPE_ARRAY,
	NEREON_TYPE_IPPORT,
	NEREON_TYPE_FLOAT,
	NEREON_TYPE_OBJECT,
	NEREON_TYPE_UNKNOWN
};

/*
 * multiconfig configuration data
 */

union _cfg_data {
	int i;
	char *str;
	bool b;
	double d;
};

#endif /* __NEREON_COMMON_H__ */

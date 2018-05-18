#ifndef __MCONFIG_COMMON_H__
#define __MCONFIG_COMMON_H__

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

enum MCFG_TYPE {
	MCFG_TYPE_INT = 0,
	MCFG_TYPE_BOOL,
	MCFG_TYPE_STRING,
	MCFG_TYPE_ARRAY,
	MCFG_TYPE_IPPORT,
	MCFG_TYPE_FLOAT,
	MCFG_TYPE_OBJECT,
	MCFG_TYPE_UNKNOWN
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

#endif /* __MCONFIG_COMMON_H__ */

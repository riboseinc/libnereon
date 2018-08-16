#ifndef __RTR2_CONF_TEST_H__
#define __RTR2_CONF_TEST_H__

#define RTR2_MAX_PATH                    512
#define RTR2_FUNC_MAX_NAME               128
#define RTR2_ACTION_MAX_NAME             128
#define RTR2_PARAM_MAX_NAME              128

/*
 * retrace v2 log level
 */

enum RTR2_LOG_LEVEL {
	RTR2_LOG_LEVEL_SILENT = 1,
	RTR2_LOG_LEVEL_NORMAL,
	RTR2_LOG_LEVEL_DEBUG,
	RTR2_LOG_LEVEL_VERBOSE,
};

/*
 * retrace v2 match types
 */

enum RTR2_MATCH_TYPE {
	RTR2_MATCH_TYPE_INT = 0,
	RTR2_MATCH_TYPE_STRING,
	RTR2_MATCH_TYPE_ARRAY
};

/*
 * retrace v2 action info
 */

typedef struct rtr2_action {
	char *name;

	char *param_name;
	enum RTR2_MATCH_TYPE match_type;

	void *match_data;
	void *new_data;
} rtr2_action_t;

/*
 * retrace v2 function info
 */

typedef struct rtr2_func {
	char *name;

	rtr2_action_t *actions;
	int actions_count;

	enum RTR2_LOG_LEVEL log_level;                       /* if log_level is not set, then it's same with global one */
} rtr2_func_t;

/*
 * retrace v2 configuration
 */

typedef struct rtr2_conf {
	char *log_path;
	enum RTR2_LOG_LEVEL log_level;

	rtr2_func_t *funcs;
	int funcs_count;
} rtr2_conf_t;

#endif /* __RTR2_CONF_TEST_H__ */

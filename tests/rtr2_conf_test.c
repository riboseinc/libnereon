/*
 * Copyright (c) 2018, [Ribose Inc](https://www.ribose.com).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "nereon.h"

#include "rtr2_conf_test.h"

/*
 * print retrace2 configuration options
 */

static void print_rtr2_config_options(rtr2_conf_t *config)
{
	fprintf(stdout, "global.log_level: %d\n", config->log_level);
	fprintf(stdout, "global.log_path : %s\n", config->log_path);
}

/*
 * get action list
 */

static int get_actions_list(nereon_config_object_t obj, rtr2_func_t *func)
{
	rtr2_action_t *actions = NULL;
	int actions_count = 0;

	int ret = 0;

	const char *key = "action";

	/* parse action objects */
	while (obj) {
		nereon_object_object_foreach(obj, key, p) {
			rtr2_action_t action;
			char *match_str = NULL, *new_str = NULL;

			nereon_config_option_t action_opts[] = {
				{NULL, NEREON_TYPE_KEY, false, NULL, &action.name},
				{"param_name", NEREON_TYPE_STRING, false, NULL, &action.param_name},
				{"match_type", NEREON_TYPE_INT, false, NULL, &action.match_type},
				{"match_str", NEREON_TYPE_STRING, false, NULL, &match_str},
				{"new_str", NEREON_TYPE_ARRAY, false, NULL, &new_str},
			};

			if (!p)
				break;

			/* get function info from object */
			memset(&action, 0, sizeof(action));
			if (nereon_object_config_options(p, action_opts) != 0) {
				fprintf(stderr, "Failed to get action options(err:%s)\n", nereon_get_errmsg());
				ret = -1;

				break;
			}

			actions = realloc(actions, (actions_count + 1) * sizeof(rtr2_action_t));
			if (!actions) {
				fprintf(stderr, "Out of memory!\n");
				ret = -1;

				break;
			}
			memcpy(&actions[actions_count], &action, sizeof(rtr2_action_t));
			actions_count++;
		}
	}

	if (ret != 0 && actions)
		free(actions);
	else {
		func->actions = actions;
		func->actions_count = actions_count;
	}

	return ret;
}

/*
 * get interception function list
 */

static int get_intercept_funcs_list(nereon_config_object_t obj, rtr2_conf_t *config)
{
	rtr2_func_t *funcs = NULL;
	int funcs_count = 0;

	int ret = 0;

	const char *key = "interception_func";

	/* parse intercept function objects */
	while (obj) {
		nereon_object_object_foreach(obj, key, p) {
			rtr2_func_t func;
			nereon_config_object_t act_objs;

			nereon_config_option_t func_opts[] = {
				{NULL, NEREON_TYPE_KEY, false, NULL, &func.name},
				{"log_level", NEREON_TYPE_STRING, false, NULL, &func.log_level},
				{"action", NEREON_TYPE_ARRAY, false, NULL, &act_objs},
			};

			if (!p)
				break;

			/* get function info from object */
			memset(&func, 0, sizeof(func));
			if (nereon_object_config_options(p, func_opts) != 0) {
				fprintf(stderr, "Failed to get function options(err:%s)\n", nereon_get_errmsg());
				ret = -1;

				break;
			}

			funcs = realloc(funcs, (funcs_count + 1) * sizeof(rtr2_func_t));
			if (!funcs) {
				fprintf(stderr, "Out of memory!\n");
				ret = -1;

				break;
			}
			memcpy(&funcs[funcs_count], &func, sizeof(rtr2_func_t));
			funcs_count++;
		}
	}

	if (ret != 0 && funcs)
		free(funcs);
	else {
		config->funcs = funcs;
		config->funcs_count = funcs_count;
	}

	return ret;
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	nereon_ctx_t ctx;
	rtr2_conf_t rtr2_config;

	nereon_config_object_t intercept_funcs = NULL;

	int ret, i;

	nereon_config_option_t rtr2_cfg_opts[] = {
		{"globals.log_path", NEREON_TYPE_STRING, false, NULL, &rtr2_config.log_path},
		{"globals.log_level", NEREON_TYPE_STRING, false, NULL, &rtr2_config.log_level},
		{"interception_func", NEREON_TYPE_ARRAY, false, NULL, &intercept_funcs}
	};

	if (argc != 2) {
		fprintf(stderr, "Usage: rtr2_conf_test <retrace V2 configuration file>\n");
		exit(1);
	}

	/* initialize nereon context */
	ret = nereon_ctx_init(&ctx, NULL);
	if (ret != 0) {
		fprintf(stderr, "Could not initialize libnereon context(err:%s)\n", nereon_get_errmsg());
		return -1;
	}

	/* parse configuration file */
	memset(&rtr2_config, 0, sizeof(rtr2_config));

	fprintf(stderr, "Try to parse configuration file '%s'\n", argv[1]);

	ret = nereon_parse_config_file(&ctx, argv[1]);
	if (ret != 0) {
		fprintf(stderr, "Failed to parse configuration file '%s'(err:%s)\n", argv[1], nereon_get_errmsg());
		goto end;
	}

	/* get configuration options */
	ret = nereon_get_config_options(&ctx, rtr2_cfg_opts);
	if (ret != 0) {
		fprintf(stderr, "Failed to get configuration options(err:%s)\n", nereon_get_errmsg());
		goto end;
	}

	/* get interception function list */
	ret = get_intercept_funcs_list(intercept_funcs, &rtr2_config);

	/* print retrace v2 configuration options */
	print_rtr2_config_options(&rtr2_config);

end:
	nereon_ctx_finalize(&ctx);

	return ret;
}

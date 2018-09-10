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

rtr2_conf_t *g_rtr2_config;
static nereon_ctx_t g_nereon_ctx;

/*
 * print retrace2 configuration options
 */

static void print_rtr2_config_options()
{
	int i;

	fprintf(stdout, "global.log_level: %d\n", g_rtr2_config->log_level);
	fprintf(stdout, "global.log_path : %s\n", g_rtr2_config->log_path);

	for (i = 0; i < g_rtr2_config->funcs_count; i++) {
		int j;

		fprintf(stdout, "#%d-func.name: %s\n", i, g_rtr2_config->funcs[i].name);
		fprintf(stdout, "#%d-func.log_level: %d\n", i, g_rtr2_config->funcs[i].log_level);
		for (j = 0; j < g_rtr2_config->funcs[i].actions_count; j++) {
			rtr2_action_t *action = &g_rtr2_config->funcs[i].actions[j];

			fprintf(stdout, "\t%d-action.name: %s\n", j, action->name);
			fprintf(stdout, "\t%d-action.param_name: %s\n", j, action->param_name);
			fprintf(stdout, "\t%d-action.match_type: %d\n", j, action->match_type);

			switch (action->match_type) {
			case RTR2_MATCH_TYPE_INT:
			case RTR2_MATCH_TYPE_ANY_INT:
			case RTR2_MATCH_TYPE_CHAR:
				{
					if (action->match_type == RTR2_MATCH_TYPE_ANY_INT)
						fprintf(stdout, "\t%d-action.match_data: ANY\n", j);
					else
						fprintf(stdout, "\t%d-action.match_data: %d\n", j, *(int *)action->match_data);

					fprintf(stdout, "\t%d-action.new_data: %d\n", j, *(int *)action->new_data);
				}
				break;

			case RTR2_MATCH_TYPE_STRING:
			case RTR2_MATCH_TYPE_ANY_STRING:
				{
					if (action->match_type == RTR2_MATCH_TYPE_ANY_STRING)
						fprintf(stderr, "\t%d-action.match_data: ANY\n", j);
					else
						fprintf(stdout, "\t%d-action.match_data: %s\n", j, (char *)action->match_data);

					fprintf(stderr, "\t%d-action.new_data: %s\n", j, (char *)action->new_data);
				}
				break;

			case RTR2_MATCH_TYPE_ANY_RANDOM:
				{
					fprintf(stdout, "\t%d-action.match_data: ANY\n", j);
					fprintf(stdout, "\t%d-action.new_data: RANDOM\n", j);
				}
				break;

			case RTR2_MATCH_TYPE_ARRAY_INT:
				{
					int k;

					fprintf(stdout, "\t%d-action.match_data: ", j);
					for (k = 0; k < action->match_data_num; k++)
						fprintf(stdout, "%d,", ((int *)action->match_data)[k]);
					fprintf(stdout, "\n");

					fprintf(stdout, "\t%d-action.new_data: ", j);
					for (k = 0; k < action->new_data_num; k++)
						fprintf(stdout, "%d,", ((int *)action->new_data)[k]);
					fprintf(stdout, "\n");
				}
				break;

			case RTR2_MATCH_TYPE_ARRAY_STRING:
				{
					int k;

					fprintf(stdout, "\t%d-action.match_data: ", j);
					for (k = 0; k < action->match_data_num; k++)
						fprintf(stdout, "%s,", ((char **)action->match_data)[k]);
					fprintf(stdout, "\n");

					fprintf(stdout, "\t%d-action.new_data: ", j);
					for (k = 0; k < action->new_data_num; k++)
						fprintf(stdout, "%s,", ((char **)action->new_data)[k]);
					fprintf(stdout, "\n");
				}
				break;

			default:
				break;
			}
		}
	}
}

/*
 * get match type from string
 */

static struct rtr2_match_types {
	enum RTR2_MATCH_TYPE match_type;
	const char *str;
} g_match_types[] = {
	{RTR2_MATCH_TYPE_STRING, "string"},
	{RTR2_MATCH_TYPE_INT, "int"},
	{RTR2_MATCH_TYPE_ARRAY_INT, "array_int"},
	{RTR2_MATCH_TYPE_ARRAY_STRING, "array_string"},
	{RTR2_MATCH_TYPE_CHAR, "char"},
	{RTR2_MATCH_TYPE_UNKNOWN, NULL}
};

static enum RTR2_MATCH_TYPE get_match_type(const char *type_str)
{
	int i;

	for (i = 0; g_match_types[i].str != NULL; i++) {
		if (strcmp(g_match_types[i].str, type_str) == 0)
			return g_match_types[i].match_type;
	}

	return RTR2_MATCH_TYPE_UNKNOWN;
}

/*
 * set action array data
 */

static int set_action_array_data(nereon_noc_option_t *noc_opt, enum RTR2_MATCH_TYPE match_type, void **action_data, int *num)
{
	void *data = NULL;
	int opt_count = 0;

	/* set action data */
	nereon_object_object_foreach(noc_opt, val) {
		if (match_type == RTR2_MATCH_TYPE_ARRAY_INT) {
			data = realloc(data, (opt_count + 1) * sizeof(int));
			if (!data)
				goto err;

			((int *)data)[opt_count++] = val->data.i;
		} else {
			data = realloc(data, (opt_count + 1) * sizeof(char **));
			if (!data)
				goto err;

			((char **)data)[opt_count++] = val->data.str;
		}
	}
	*action_data = data;
	*num = opt_count;

	return 0;

err:
	if (data)
		free(data);

	return -1;
}

/*
 * set match data
 */

static int set_action_data(nereon_noc_option_t *match_opt, nereon_noc_option_t *new_opt, rtr2_action_t *action)
{
	int ret = 0;

	fprintf(stderr, "set action data for action:%s\n", action->name);

	/* check whether special types are used */
	if (match_opt->type == NEREON_TYPE_STRING && strcmp(match_opt->data.str, "any") == 0) {
		if (new_opt->type == NEREON_TYPE_STRING && strcmp(new_opt->data.str, "random") == 0)
			action->match_type = RTR2_MATCH_TYPE_ANY_RANDOM;
		else {
			if (action->match_type == RTR2_MATCH_TYPE_INT)
				action->match_type = RTR2_MATCH_TYPE_ANY_INT;
			else
				action->match_type = RTR2_MATCH_TYPE_ANY_STRING;
		}

		return 0;
	}

	switch (action->match_type) {
	case RTR2_MATCH_TYPE_INT:
	case RTR2_MATCH_TYPE_CHAR:
		{
			if (match_opt->type != NEREON_TYPE_INT || new_opt->type != NEREON_TYPE_INT) {
				ret = -1;
				break;
			}
			*(int *)(action->match_data) = match_opt->data.i;
			*(int *)(action->new_data) = new_opt->data.i;
		}
		break;

	case RTR2_MATCH_TYPE_STRING:
		{
			if (match_opt->type != NEREON_TYPE_STRING || new_opt->type != NEREON_TYPE_STRING) {
				ret = -1;
				break;
			}
			action->match_data = (void *)match_opt->data.str;
			action->new_data = (void *)new_opt->data.str;
		}
		break;

	case RTR2_MATCH_TYPE_ARRAY_INT:
	case RTR2_MATCH_TYPE_ARRAY_STRING:
		{
			if (set_action_array_data(match_opt, action->match_type, &action->match_data, &action->match_data_num) != 0 ||
				set_action_array_data(new_opt, action->match_type, &action->new_data, &action->new_data_num) != 0) {
				ret = -1;
				break;
			}
		}
		break;

	default:
		break;
	}

	return ret;
}

/*
 * get action list
 */

static int get_actions_list(rtr2_func_t *func, nereon_noc_option_t *noc_opt)
{
	rtr2_action_t *actions = NULL;
	int actions_count = 0;

	int ret = 0;

	fprintf(stderr, "Try to find actions from object '%p'\n", noc_opt);

	/* parse action objects */
	nereon_object_object_foreach(noc_opt, p) {
		rtr2_action_t action;
		nereon_noc_option_t *match_data = NULL, *new_data = NULL;
		char *match_type;

		nereon_config_option_t action_opts[] = {
			{NULL, NEREON_TYPE_KEY, false, NULL, &action.name},
			{"param_name", NEREON_TYPE_STRING, false, NULL, &action.param_name},
			{"match_type", NEREON_TYPE_STRING, false, NULL, &match_type},
			{"match_data", NEREON_TYPE_OBJECT, false, NULL, &match_data},
			{"new_data", NEREON_TYPE_OBJECT, false, NULL, &new_data},
		};

		if (!p)
			break;

		if (strcmp(p->key, "action") != 0)
			continue;

		/* get function info from object */
		memset(&action, 0, sizeof(action));
		if (nereon_get_noc_configs(p->childs, action_opts) != 0) {
			fprintf(stderr, "Failed to get action options(err:%s)\n", nereon_get_errmsg());
			ret = -1;

			break;
		}

		/* set match type */
		action.match_type = get_match_type(match_type);

		/* set match and new data */
		if (set_action_data(match_data, new_data, &action) != 0) {
			fprintf(stderr, "Invalid match data for action '%s'\n", action.name);
			ret = -1;

			break;
		}

		/* add action to list */
		actions = realloc(actions, (actions_count + 1) * sizeof(rtr2_action_t));
		if (!actions) {
			fprintf(stderr, "Out of memory!\n");
			ret = -1;

			break;
		}
		memcpy(&actions[actions_count], &action, sizeof(rtr2_action_t));
		actions_count++;
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

static int get_intercept_funcs_list(rtr2_conf_t *config, nereon_noc_option_t *noc_opt)
{
	rtr2_func_t *funcs = NULL;
	int funcs_count = 0;

	int ret = 0;

	/* parse intercept function objects */
	nereon_object_object_foreach(noc_opt, p) {
		rtr2_func_t func;
		nereon_noc_option_t *act_objs = NULL;

		nereon_config_option_t func_opts[] = {
			{NULL, NEREON_TYPE_KEY, false, NULL, &func.name},
			{"log_level", NEREON_TYPE_INT, false, NULL, &func.log_level},
			{"action", NEREON_TYPE_ARRAY, false, NULL, &act_objs},
		};

		if (strcmp(p->key, "interception_func") != 0)
			continue;

		/* get function info from object */
		memset(&func, 0, sizeof(func));
		if (nereon_get_noc_configs(p->childs, func_opts) != 0) {
			fprintf(stderr, "Failed to get function options(err:%s)\n", nereon_get_errmsg());
			ret = -1;

			break;
		}

		/* get action info from action object */
		get_actions_list(&func, act_objs->childs);

		funcs = realloc(funcs, (funcs_count + 1) * sizeof(rtr2_func_t));
		if (!funcs) {
			fprintf(stderr, "Out of memory!\n");
			ret = -1;

			break;
		}
		memcpy(&funcs[funcs_count], &func, sizeof(rtr2_func_t));
		funcs_count++;
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
 * free rtr2 config
 */

static void rtr2_config_free(void)
{
	int i;

	if (!g_rtr2_config)
		return;

	if (!g_rtr2_config->funcs) {
		free(g_rtr2_config);
		return;
	}

	for (i = 0; i < g_rtr2_config->funcs_count; i++) {
		rtr2_func_t *func = &g_rtr2_config->funcs[i];
		int j;

		if (!func->actions)
			continue;

		for (j = 0; j < func->actions_count; j++) {
			rtr2_action_t *action = &func->actions[j];

			if (action->match_type != RTR2_MATCH_TYPE_ARRAY_INT &&
				action->match_type != RTR2_MATCH_TYPE_ARRAY_STRING)
				continue;

			if (action->match_data)
				free(action->match_data);

			if (action->new_data)
				free(action->new_data);
		}
		free(func->actions);
	}

	free(g_rtr2_config->funcs);
	free(g_rtr2_config);

	nereon_ctx_finalize(&g_nereon_ctx);
}

/*
 * initiailze rtr2 config
 */

static int rtr2_conf_init(const char *config_path)
{
	nereon_noc_option_t *funcs = NULL;
	int ret, i;

	char *log_path = NULL, *log_level = NULL;

	nereon_config_option_t rtr2_cfg_opts[] = {
		{"globals.log_path", NEREON_TYPE_STRING, false, NULL, &log_path},
		{"globals.log_level", NEREON_TYPE_STRING, false, NULL, &log_level},
		{"interception_func", NEREON_TYPE_ARRAY, false, NULL, &funcs}
	};

	/* allocate memory for rtr2 config */
	g_rtr2_config = (rtr2_conf_t *)malloc(sizeof(rtr2_conf_t));
	if (!g_rtr2_config) {
		fprintf(stderr, "Out of memory!\n");
		return -1;
	}
	memset(g_rtr2_config, 0, sizeof(rtr2_conf_t));

	/* initialize nereon context */
	ret = nereon_ctx_init(&g_nereon_ctx, NULL);
	if (ret != 0) {
		fprintf(stderr, "Could not initialize libnereon context(err:%s)\n", nereon_get_errmsg());
		goto err;
	}

	fprintf(stderr, "Try to parse configuration file '%s'\n", config_path);

	ret = nereon_parse_config_file(&g_nereon_ctx, config_path);
	if (ret != 0) {
		fprintf(stderr, "Failed to parse configuration file '%s'(err:%s)\n", config_path, nereon_get_errmsg());
		goto err;
	}

	/* get configuration options */
	ret = nereon_get_config_options(&g_nereon_ctx, rtr2_cfg_opts);
	if (ret != 0) {
		fprintf(stderr, "Failed to get configuration options(err:%s)\n", nereon_get_errmsg());
		goto err;
	}

	/* get interception function list */
	ret = get_intercept_funcs_list(g_rtr2_config, funcs);
	if (ret != 0)
		goto err;

	return 0;

err:
	nereon_ctx_finalize(&g_nereon_ctx);
	rtr2_config_free();

	return -1;
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: rtr2_conf_test <retrace V2 configuration file>\n");
		exit(1);
	}

	/* initialize nereon context */
	if (rtr2_conf_init(argv[1]) != 0)
		exit(1);

	/* print retrace v2 configuration options */
	print_rtr2_config_options();

	/* free rtr2 configuration buffers */
	rtr2_config_free();

	return 0;
}

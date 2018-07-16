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
#include <ucl.h>

#include "common.h"
#include "err.h"
#include "util.h"
#include "nereon.h"

#include "nos.h"

static const char *nos_types[] = {
	"INT", "BOOL", "STRING", "ARRAY", "IPPORT", "FLOAT",
	"OBJECT", "CONFIG", "HELPER", "UNKNOWN"
};

/*
 * print NOS options
 */

static void print_nos_options(struct nereon_nos_option *nos_opts, int nos_opts_count)
{
	int i;

	DEBUG_PRINT("Printing NOS options\n");

	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_option *opt = &nos_opts[i];

		DEBUG_PRINT("\tname: %s\n", opt->name);
		DEBUG_PRINT("\t\ttype: %s\n", nos_types[opt->type]);
		DEBUG_PRINT("\t\tshort_sw: %s\n", opt->sw_short);
		DEBUG_PRINT("\t\tlong_sw: %s\n", opt->sw_long);
		DEBUG_PRINT("\t\tshort_desc: %s\n", opt->desc_short);
		DEBUG_PRINT("\t\tlong_desc: %s\n", opt->desc_long);
		DEBUG_PRINT("\t\tenv: %s\n", opt->desc_short);
		DEBUG_PRINT("\t\tconfig: %s\n", opt->noc_key);
		DEBUG_PRINT("\t\texist_default: %s\n", opt->exist_default ? "yes" : "no");
	}
}

/*
 * parse option types
 */

static enum NEREON_CONFIG_TYPE parse_opt_type(const char *type_str)
{
	enum NEREON_CONFIG_TYPE type = NEREON_TYPE_UNKNOWN;

	if (strcmp(type_str, "string") == 0)
		type = NEREON_TYPE_STRING;
	else if (strcmp(type_str, "int") == 0)
		type = NEREON_TYPE_INT;
	else if (strcmp(type_str, "array") == 0)
		type = NEREON_TYPE_ARRAY;
	else if (strcmp(type_str, "ipport") == 0)
		type = NEREON_TYPE_IPPORT;
	else if (strcmp(type_str, "bool") == 0)
		type = NEREON_TYPE_BOOL;
	else if (strcmp(type_str, "config") == 0)
		type = NEREON_TYPE_CONFIG;
	else if (strcmp(type_str, "helper") == 0)
		type = NEREON_TYPE_HELPER;

	return type;
}

/*
 * set options
 */

static int set_default_value(const ucl_object_t *obj, struct nereon_nos_option *opt)
{
	DEBUG_PRINT("Setting NOS default data for option '%s'\n", opt->name);

	if (opt->type == NEREON_TYPE_INT && obj->type == UCL_INT) {
		opt->default_data.i = ucl_object_toint(obj);
		DEBUG_PRINT("\tSetting default value '%d' for option '%s'\n", opt->default_data.i, opt->name);
	} else if (opt->type == NEREON_TYPE_BOOL && obj->type == UCL_BOOLEAN) {
		opt->default_data.b = ucl_object_toboolean(obj);
		DEBUG_PRINT("\tSetting default value '%s' for option '%s'\n",
			opt->default_data.b ? "true" : "false", opt->name);
	} else if ((opt->type == NEREON_TYPE_STRING || opt->type == NEREON_TYPE_IPPORT ||
		opt->type == NEREON_TYPE_CONFIG) &&
		obj->type == UCL_STRING) {
		char *str;

		str = strdup(ucl_object_tostring(obj));
		if (!str) {
			return -1;
		}

		DEBUG_PRINT("\tSetting default value '%s' for option '%s'\n", str, opt->name);

		opt->default_data.str = str;
	} else {
		DEBUG_PRINT("\tInvalid default value for config_option '%s'\n", opt->name);
		return -1;
	}

	return 0;
}

/*
 * parse switch options
 */

static void parse_switch_options(const ucl_object_t *obj, struct nereon_nos_option *opt)
{
	const ucl_object_t *sub_obj;

	sub_obj = ucl_object_lookup(obj, "short");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strcpy_s(opt->sw_short, ucl_object_tostring(sub_obj), sizeof(opt->sw_short));
	}

	sub_obj = ucl_object_lookup(obj, "long");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strcpy_s(opt->sw_long, ucl_object_tostring(sub_obj), sizeof(opt->sw_long));
	}
}

/*
 * parse description options
 */

static void parse_desc_options(const ucl_object_t *obj, struct nereon_nos_option *opt)
{
	const ucl_object_t *sub_obj;

	sub_obj = ucl_object_lookup(obj, "short");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strcpy_s(opt->desc_short, ucl_object_tostring(sub_obj), sizeof(opt->desc_short));
	}

	sub_obj = ucl_object_lookup(obj, "long");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strcpy_s(opt->desc_long, ucl_object_tostring(sub_obj), sizeof(opt->desc_long));
	}
}

/*
 * parse cmdline arguments
 */

static void parse_cmdline_arguments(const ucl_object_t *obj, struct nereon_nos_option *opt)
{
	const ucl_object_t *sub_obj, *tmp;
	ucl_object_iter_t it = NULL;

	int ret = 0;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, true))) {
		struct nereon_nos_option arg;

		sub_obj = ucl_object_lookup(obj, "type");
		if (!sub_obj)
			continue;

		/* set argument */
		memset(&arg, 0, sizeof(struct nereon_nos_option));

		strcpy_s(arg.name, obj->key, sizeof(arg.name));
		arg.type = parse_opt_type(ucl_object_tostring(sub_obj));

		opt->cli_args = realloc(opt->cli_args, (opt->cli_args_count + 1) * sizeof(struct nereon_nos_option));
		if (!opt->cli_args) {
			opt->cli_args_count = 0;
			return;
		}

		memcpy(&opt->cli_args[opt->cli_args_count++], &arg, sizeof(struct nereon_nos_option));
	}
}

/*
 * parse cmdline options
 */

static void parse_cmdline_options(const ucl_object_t *obj, struct nereon_nos_option *opt)
{
	const ucl_object_t *tmp;
	ucl_object_iter_t it = NULL;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, false))) {
		const ucl_object_t *sub_obj;

		/* parse switch options */
		sub_obj = ucl_object_lookup(obj, "switch");
		if (sub_obj && sub_obj->type == UCL_OBJECT) {
			parse_switch_options(sub_obj, opt);
			continue;
		}

		/* parse description options */
		sub_obj = ucl_object_lookup(obj, "description");
		if (sub_obj && sub_obj->type == UCL_OBJECT) {
			parse_desc_options(sub_obj, opt);
			continue;
		}

		/* parse arguments */
		sub_obj = ucl_object_lookup(obj, "arguments");
		if (sub_obj && sub_obj->type == UCL_OBJECT) {
			parse_cmdline_arguments(sub_obj, opt);
			continue;
		}
	}
}

/*
 * parse options
 */

static int parse_config_option(const ucl_object_t *obj, struct nereon_nos_option *opt)
{
	const ucl_object_t *sub_obj;

	/* set key */
	strcpy_s(opt->name, obj->key, sizeof(opt->name));

	/* get type */
	sub_obj = ucl_object_lookup(obj, "type");
	if (!sub_obj || sub_obj->type != UCL_STRING) {
		nereon_set_err("Could not find 'type' key for config '%s'", opt->name);
		return -1;
	}

	opt->type = parse_opt_type(ucl_object_tostring(sub_obj));
	if (opt->type == NEREON_TYPE_UNKNOWN) {
		nereon_set_err("Invalid configuration type '%s' for config '%s'", ucl_object_tostring(sub_obj), opt->name);
		return -1;
	}

	/* get switch */
	sub_obj = ucl_object_lookup(obj, "cmdline");
	if (sub_obj && sub_obj->type == UCL_OBJECT) {
		parse_cmdline_options(sub_obj, opt);
	}

	/* get env */
	sub_obj = ucl_object_lookup(obj, "env");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strcpy_s(opt->env, ucl_object_tostring(sub_obj), sizeof(opt->env));
	}

	/* get config */
	sub_obj = ucl_object_lookup(obj, "config");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strcpy_s(opt->noc_key, ucl_object_tostring(sub_obj), sizeof(opt->noc_key));
	}

	/* get default */
	sub_obj = ucl_object_lookup(obj, "default");
	if (sub_obj && set_default_value(sub_obj, opt) == 0) {
		opt->exist_default = true;
	}

	return 0;
}

/*
 * parse cmdline options
 */

static int parse_nereon_options(const ucl_object_t *obj, struct nereon_nos_option **nos_opts, int *nos_opts_count)
{
	const ucl_object_t *tmp, *cur;
	ucl_object_iter_t it = NULL, it_obj = NULL;

	int ret = 0;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, false)) && ret == 0) {
		if (strcmp(obj->key, "config_option") == 0 && obj->type == UCL_OBJECT) {
			it_obj = NULL;
			while ((cur = ucl_object_iterate(obj, &it_obj, true)) && ret == 0) {
				struct nereon_nos_option opt;

				DEBUG_PRINT("parse_nereon_options() key:%s\n", cur->key);

				memset(&opt, 0, sizeof(struct nereon_nos_option));
				if (parse_config_option(cur, &opt) == 0) {
					*nos_opts = (struct nereon_nos_option *)realloc(*nos_opts,
								(*nos_opts_count + 1) * sizeof(struct nereon_nos_option));
					if (*nos_opts == NULL) {
						nereon_set_err("Out of memory!");
						ret = -1;
					} else {
						memcpy(&(*nos_opts)[*nos_opts_count], &opt, sizeof(struct nereon_nos_option));
						(*nos_opts_count)++;
					}
				} else {
					if (*nos_opts)
						free(*nos_opts);

					ret = -1;
				}
			}
		} else
			ret = -1;
	}

	return ret;
}

/*
 * parse HCL options
 */

static int parse_nos_options(const ucl_object_t *obj, struct nereon_nos_option **nos_opts, int *nos_opts_count)
{
	const ucl_object_t *tmp, *cur;
	ucl_object_iter_t it = NULL, it_obj = NULL;

	struct nereon_nos_option *opt;

	int ret = 0;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, false)) && ret == 0) {
		if (obj->type == UCL_OBJECT) {
			it_obj = NULL;
			while ((cur = ucl_object_iterate(obj, &it_obj, true)) && ret == 0) {
				ret = parse_nereon_options(cur, nos_opts, nos_opts_count);
			}
		} else
			ret = -1;
	}

	return ret;
}

/*
 * parse HCL options
 */

int nereon_parse_nos_options(const char *nos_cfg, struct nereon_nos_option **nos_opts, int *nos_opts_count)
{
	struct ucl_parser *parser;
	ucl_object_t *obj = NULL;

	int ret;

	DEBUG_PRINT("Parsing NOS options\n");

	/* create UCL object from HCL string */
	parser = ucl_parser_new(0);
	if (!parser) {
		nereon_set_err("Could not create UCL parser");
		return -1;
	}
	ucl_parser_add_chunk(parser, (const unsigned char*)nos_cfg, strlen(nos_cfg));

	if (ucl_parser_get_error(parser) != NULL) {
		nereon_set_err("Failed to parse HCL options(%s)", ucl_parser_get_error(parser));
		ucl_parser_free(parser);

		return -1;
	}

	obj = ucl_parser_get_object(parser);
	if (!obj) {
		nereon_set_err("Failed to get HCL object(%s)", ucl_parser_get_error(parser));
		ucl_parser_free(parser);

		return -1;
	}

	/* parse HCL options */
	ret = parse_nos_options(obj, nos_opts, nos_opts_count);

	/* free object and parser */
	ucl_object_unref(obj);
	ucl_parser_free(parser);

	if (ret == 0)
		print_nos_options(*nos_opts, *nos_opts_count);

	return ret;
}

/*
 * free NOS options
 */

void nereon_free_nos_options(struct nereon_nos_option *nos_opts, int nos_opts_count)
{
	int i;

	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_option *opt = &nos_opts[i];

		if (opt->type == NEREON_TYPE_STRING || opt->type == NEREON_TYPE_IPPORT) {
			if (opt->data.str)
				free(opt->data.str);

			if (opt->exist_default && opt->default_data.str)
				free(opt->default_data.str);
		}

		if (opt->cli_args_count > 0) {
			nereon_free_nos_options(opt->cli_args, opt->cli_args_count);
		}
	}

	free(nos_opts);
}

/*
 * get nos option
 */

struct nereon_nos_option *nereon_get_nos_option(struct nereon_nos_option *nos_opts, int nos_opts_count,	const char *key)
{
	int i;

	DEBUG_PRINT("Try to get NOS option for config '%s'\n", key);

	for (i = 0; i < nos_opts_count; i++) {
		struct nereon_nos_option *opt = &nos_opts[i];

		if (strcasecmp(opt->name, key) == 0) {
			return opt;
		}
	}

	return NULL;
}

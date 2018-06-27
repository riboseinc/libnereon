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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <ucl.h>

#include "common.h"
#include "err.h"
#include "util.h"
#include "nereon.h"
#include "nos.h"

#include "noc.h"

/*
 * free config option
 */

static void free_config_options(struct nereon_noc_option *opt)
{
	if (opt) {
		free_config_options(opt->childs);
		free_config_options(opt->next);
		if (opt->type == NEREON_TYPE_STRING && opt->data.str)
			free(opt->data.str);

		free(opt);
	}
}

/*
 * print config options
 */

static const char *convert_to_str(enum NEREON_CONFIG_TYPE type, union nereon_config_data *data)
{
	static char str_data[512];

	if (type == NEREON_TYPE_INT) {
		snprintf(str_data, sizeof(str_data), "%d", data->i);
	} else if (type == NEREON_TYPE_FLOAT) {
		snprintf(str_data, sizeof(str_data), "%f", data->d);
	} else if (type == NEREON_TYPE_STRING) {
		strcpy_s(str_data, data->str, sizeof(str_data));
	} else if (type == NEREON_TYPE_BOOL) {
		strcpy_s(str_data, data->b ? "true" : "false", sizeof(str_data));
	} else
		str_data[0] = '\0';

	return str_data;
}

static void print_noc_options(struct nereon_noc_option *opt, int level)
{
	char *sp_str = NULL;

	if (!opt)
		return;

	/* create space string */
	if (level > 0) {
		sp_str = (char *)malloc(level * 4 + 1);
		if (!sp_str)
			return;
		memset(sp_str, ' ', level * 4);
		sp_str[level * 4] = '\0';

		DEBUG_PRINT("%sopt->key:%s, opt->data:%s\n", sp_str, opt->key, convert_to_str(opt->type, &opt->data));
	}

	if (opt->childs) {
		struct nereon_noc_option *p = opt->childs;

		level++;
		while (p) {
			print_noc_options(p, level);
			p = p->next;
		}
	}

	free(sp_str);
}

/*
 * allocate memory for config option
 */

static struct nereon_noc_option *new_noc_option()
{
	struct nereon_noc_option *p;

	p = (struct nereon_noc_option *)malloc(sizeof(struct nereon_noc_option));
	if (!p) {
		nereon_set_err("Could not create UCL parser");
		return NULL;
	}

	memset(p, 0, sizeof(struct nereon_noc_option));

	return p;
}

/*
 * add config option to list
 */

static void add_noc_option(struct nereon_noc_option *parent_opt, struct nereon_noc_option *opt)
{
	if (!parent_opt->childs) {
		parent_opt->childs = opt;
	} else {
		struct nereon_noc_option *p = parent_opt->childs;

		while (p->next)
			p = p->next;

		p->next = opt;
	}
}

/*
 * set value for config option
 */

static int set_noc_value(const ucl_object_t *obj, struct nereon_noc_option *opt)
{
	if (obj->type == UCL_INT) {
		opt->type = NEREON_TYPE_INT;
		opt->data.i = ucl_object_toint(obj);
	} else if (obj->type == UCL_FLOAT) {
		opt->type = NEREON_TYPE_FLOAT;
		opt->data.d = ucl_object_todouble(obj);
	} else if (obj->type == UCL_STRING) {
		opt->type = NEREON_TYPE_STRING;
		opt->data.str = strdup(ucl_object_tostring(obj));
	} else if (obj->type == UCL_BOOLEAN) {
		opt->type = NEREON_TYPE_BOOL;
		opt->data.b = ucl_object_toboolean(obj);
	} else
		return -1;

	return 0;
}

/*
 * parse configuration options from UCL object
 */

int parse_config_options(const ucl_object_t *obj, struct nereon_noc_option *parent_opt)
{
	const ucl_object_t *cur, *tmp;
	ucl_object_iter_t it = NULL, it_obj = NULL;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, false))) {
		struct nereon_noc_option *opt;

		/* create HCL config from UCL object */
		opt = new_noc_option();
		if (!opt) {
			nereon_set_err("Out of memory!");
			return -1;
		}
		add_noc_option(parent_opt, opt);

		if (obj->key != NULL) {
			strcpy_s(opt->key, obj->key, sizeof(opt->key));
		}

		if (obj->type == UCL_OBJECT || obj->type == UCL_ARRAY) {
			opt->type = obj->type == UCL_OBJECT ? NEREON_TYPE_OBJECT : NEREON_TYPE_ARRAY;

			it_obj = NULL;
			while ((cur = ucl_object_iterate(obj, &it_obj, true))) {
				if (parse_config_options(cur, opt) != 0)
					return -1;
			}
		} else {
			if (set_noc_value(obj, opt) != 0) {
				nereon_set_err("Invalid configuration type for config '%s'", obj->key);
				return -1;
			}
		}
	}

	return 0;
}


/*
 * parse configuration options
 */

int nereon_parse_noc_options(const char *cfg_path, struct nereon_noc_option **noc_opts)
{
	struct ucl_parser *parser;
	ucl_object_t *obj = NULL;

	char *cfg_str;

	struct nereon_noc_option root_opt;

	int ret = -1;

	DEBUG_PRINT("Parsing configuration options\n");

	memset(&root_opt, 0, sizeof(root_opt));

	/* get configuration contents from file */
	if (read_file_contents(cfg_path, &cfg_str) == 0) {
		nereon_set_err("Could not read configuration from '%s'", cfg_path);
		return -1;
	}

	/* create UCL object from HCL string */
	parser = ucl_parser_new(0);
	if (!parser) {
		nereon_set_err("Could not create UCL parser");
		free(cfg_str);
		return -1;
	}
	ucl_parser_add_chunk(parser, (const unsigned char*)cfg_str, strlen(cfg_str));
	free(cfg_str);

	if (ucl_parser_get_error(parser) != NULL) {
		nereon_set_err("Failed to parse configuration options(%s)", ucl_parser_get_error(parser));
		goto end;
	}

	obj = ucl_parser_get_object(parser);
	if (!obj) {
		nereon_set_err("Failed to get HCL object(%s)", ucl_parser_get_error(parser));
		goto end;
	}

	/* parse HCL options */
	ret = parse_config_options(obj, &root_opt);
	if (ret == 0) {
		*noc_opts = root_opt.childs;

#ifdef DEBUG
		print_noc_options(root_opt.childs, 0);
#endif
	} else {
		free_config_options(root_opt.childs);
	}
end:
	/* free object and parser */
	if (obj)
		ucl_object_unref(obj);

	ucl_parser_free(parser);

	return ret;
}

/*
 * free configuration options
 */

void nereon_free_noc_options(struct nereon_noc_option *cfg_opts)
{
	free_config_options(cfg_opts);
}

/*
 * get NOC option
 */

struct nereon_noc_option *get_noc_option(struct nereon_noc_option *parent_opt, const char *noc_key)
{
	struct nereon_noc_option *p = parent_opt;

	DEBUG_PRINT("Getting NOC option from parent '%s' for key '%s'\n", parent_opt->key, noc_key);

	while (p) {
		if (strcmp(p->key, noc_key) == 0)
			return p;

		p = p->next;
	}

	return NULL;
}

/*
 * get NOC option
 */

struct nereon_noc_option *nereon_get_noc_option(struct nereon_noc_option *noc_opts, const char *cfg_key)
{
	struct nereon_noc_option *noc_opt = NULL;
	char *buf, *tok_key;

	DEBUG_PRINT("Try to find NOC option for config '%s'\n", cfg_key);

	buf = strdup(cfg_key);
	if (!buf) {
		nereon_set_err("Out of memory!");
		return NULL;
	}

	tok_key = strtok(buf, ".");
	while (tok_key) {
		noc_opt = get_noc_option(noc_opt ? noc_opt->childs : noc_opts->childs, tok_key);
		if (!noc_opt)
			break;

		tok_key = strtok(NULL, ".");
	}

	free(buf);

	return noc_opt;
}

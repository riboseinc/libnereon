/*
 * Copyright (c) 2017, [Ribose Inc](https://www.ribose.com).
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

#include "hcl.h"

/*
 * parse option types
 */

static enum CFG_TYPE parse_opt_type(const char *type_str)
{
	enum CFG_TYPE cfg_type = CFG_TYPE_UNKNOWN;

	if (strcmp(type_str, "string") == 0)
		cfg_type = CFG_TYPE_STRING;
	else if (strcmp(type_str, "int") == 0)
		cfg_type = CFG_TYPE_INT;
	else if (strcmp(type_str, "array") == 0)
		cfg_type = CFG_TYPE_ARRAY;
	else if (strcmp(type_str, "ipport") == 0)
		cfg_type = CFG_TYPE_IPPORT;
	else if (strcmp(type_str, "bool") == 0)
		cfg_type = CFG_TYPE_BOOL;

	return cfg_type;
}

/*
 * parse switch options
 */

static int parse_switch_options(const ucl_object_t *obj, struct mconfig_hcl_options *opt)
{
	const ucl_object_t *sub_obj;

	sub_obj = ucl_object_lookup(obj, "short");
	if (!sub_obj || sub_obj->type != UCL_STRING)
		return -1;

	strlcpy(opt->sw_short, ucl_object_tostring(sub_obj), sizeof(opt->sw_short));

	sub_obj = ucl_object_lookup(obj, "long");
	if (!sub_obj || sub_obj->type != UCL_STRING)
		return -1;

	strlcpy(opt->sw_long, ucl_object_tostring(sub_obj), sizeof(opt->sw_long));

	return 0;
}

/*
 * parse description options
 */

static int parse_desc_options(const ucl_object_t *obj, struct mconfig_hcl_options *opt)
{
	const ucl_object_t *sub_obj;

	sub_obj = ucl_object_lookup(obj, "short");
	if (!sub_obj || sub_obj->type != UCL_STRING)
		return -1;
	strlcpy(opt->desc_short, ucl_object_tostring(sub_obj), sizeof(opt->desc_short));

	sub_obj = ucl_object_lookup(obj, "long");
	if (!sub_obj || sub_obj->type != UCL_STRING)
		return -1;
	strlcpy(opt->desc_long, ucl_object_tostring(sub_obj), sizeof(opt->desc_long));

	return 0;
}

/*
 * parse options
 */

static int parse_options(const ucl_object_t *obj, struct mconfig_hcl_options *opt)
{
	const ucl_object_t *sub_obj;

	/* set key */
	strlcpy(opt->cfg_name, obj->key, sizeof(opt->cfg_name));

	/* get type */
	sub_obj = ucl_object_lookup(obj, "type");
	if (!sub_obj || sub_obj->type != UCL_STRING) {
		mconfig_set_err("Could not found 'type' key for config '%s'", opt->cfg_name);
		return -1;
	}

	opt->cfg_type = parse_opt_type(ucl_object_tostring(sub_obj));
	if (opt->cfg_type == CFG_TYPE_UNKNOWN) {
		mconfig_set_err("Invalid configuration type '%s' for config '%s'", ucl_object_tostring(sub_obj), opt->cfg_name);
		return -1;
	}

	if (opt->cfg_type == CFG_TYPE_BOOL) {
		sub_obj = ucl_object_lookup(obj, "helper");
		if (sub_obj && sub_obj->type == UCL_BOOLEAN) {
			opt->helper = ucl_object_toboolean(sub_obj);
		}
	}

	/* get switch */
	sub_obj = ucl_object_lookup(obj, "switch");
	if (!sub_obj || sub_obj->type != UCL_OBJECT) {
		mconfig_set_err("Could not found 'switch' key for config '%s'", opt->cfg_name);
		return -1;
	}

	if (parse_switch_options(sub_obj, opt) != 0) {
		mconfig_set_err("Invalid switch options for config '%s'", opt->cfg_name);
		return -1;
	}

	/* get description */
	sub_obj = ucl_object_lookup(obj, "description");
	if (!sub_obj || sub_obj->type != UCL_OBJECT) {
		mconfig_set_err("Could not found 'description' key for config '%s'", opt->cfg_name);
		return -1;
	}

	if (parse_desc_options(sub_obj, opt) != 0) {
		mconfig_set_err("Invalid description options for config '%s'", opt->cfg_name);
		return -1;
	}

	/* get env */
	sub_obj = ucl_object_lookup(obj, "env");
	if (sub_obj && sub_obj->type == UCL_STRING) {
		strlcpy(opt->cfg_env, ucl_object_tostring(sub_obj), sizeof(opt->cfg_env));
	}

	/* get config */
	sub_obj = ucl_object_lookup(obj, "config");
	if (sub_obj || sub_obj->type == UCL_STRING) {
		strlcpy(opt->cfg_key, ucl_object_tostring(sub_obj), sizeof(opt->cfg_key));
	}

	return 0;
}

/*
 * parse cmdline options
 */

static int parse_cmdline_options(const ucl_object_t *obj, struct mconfig_hcl_options **hcl_opts, int *hcl_opts_count)
{
	const ucl_object_t *tmp;
	ucl_object_iter_t it = NULL, it_obj = NULL;

	int ret = 0;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, false)) && ret == 0) {
		const ucl_object_t *cur;

		it_obj = NULL;
		while ((cur = ucl_object_iterate(obj, &it_obj, true)) && ret == 0) {
			struct mconfig_hcl_options opt;

			memset(&opt, 0, sizeof(struct mconfig_hcl_options));
			if (parse_options(cur, &opt) == 0) {
				*hcl_opts = (struct mconfig_hcl_options *)realloc(*hcl_opts,
							(*hcl_opts_count + 1) * sizeof(struct mconfig_hcl_options));
				if (*hcl_opts == NULL) {
					mconfig_set_err("Out of memory!");
					ret = -1;
				}
				else {
					memcpy(&(*hcl_opts)[*hcl_opts_count], &opt, sizeof(struct mconfig_hcl_options));
					(*hcl_opts_count)++;
				}
			} else
				ret = -1;
		}
	}

	return ret;
}

/*
 * parse HCL options
 */

static int parse_hcl_options(const ucl_object_t *obj, struct mconfig_hcl_options **hcl_opts, int *hcl_opts_count)
{
	const ucl_object_t *tmp, *cur;
	ucl_object_iter_t it = NULL, it_obj = NULL;

	struct mconfig_hcl_options *opt;

	int ret = 0;

	tmp = obj;
	while ((obj = ucl_object_iterate(tmp, &it, false)) && ret == 0) {
		if (obj->type == UCL_OBJECT) {
			it_obj = NULL;
			while ((cur = ucl_object_iterate(obj, &it_obj, true)) && ret == 0) {
				ret = parse_cmdline_options(cur, hcl_opts, hcl_opts_count);
			}
		} else
			ret = -1;
	}

	return ret;
}

/*
 * parse HCL options
 */

int mconfig_parse_hcl_options(const char *hcl, struct mconfig_hcl_options **hcl_opts, int *hcl_opts_count)
{
	struct ucl_parser *parser;
	ucl_object_t *obj = NULL;

	int ret;

	/* create UCL object from HCL string */
	parser = ucl_parser_new(0);
	if (!parser) {
		mconfig_set_err("Could not create UCL parser");
		return -1;
	}
	ucl_parser_add_chunk(parser, (const unsigned char*)hcl, strlen(hcl));

	if (ucl_parser_get_error(parser) != NULL) {
		mconfig_set_err("Failed to parse HCL options(%s)", ucl_parser_get_error(parser));
		ucl_parser_free(parser);

		return -1;
	}

	obj = ucl_parser_get_object(parser);
	if (!obj) {
		mconfig_set_err("Failed to get HCL object(%s)", ucl_parser_get_error(parser));
		ucl_parser_free(parser);

		return -1;
	}

	/* parse HCL options */
	ret = parse_hcl_options(obj, hcl_opts, hcl_opts_count);

	/* free object and parser */
	ucl_object_unref(obj);
	ucl_parser_free(parser);

	return ret;
}

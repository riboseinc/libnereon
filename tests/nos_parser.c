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
#include "nos.h"

#define BUF_SIZE                   4096

/*************************************** util.c ************************************************/

/* size bounded string copy function */
static size_t strcpy_s(char *dst, const char *src, size_t size)
{
	size_t srclen;

	/* decrease size value */
	size--;

	/* get source len */
	srclen = strlen(src);
	if (srclen > size)
		srclen = size;

	memcpy(dst, src, srclen);
	dst[srclen] = '\0';

	return srclen;
}

/*************************************** ucl_util.c ********************************************/

#define parse_ucl_fields_ex(obj, fields) \
	parse_ucl_fields(obj, fields, sizeof(fields) / sizeof(struct ucl_field))

static int parse_ucl_fields(const ucl_object_t *obj, struct ucl_field *fields, int count)
{
	int i;

	for (i = 0; i < count; i++) {
		const ucl_object_t *sub_obj;

		sub_obj = ucl_object_lookup(obj, fields[i].key);
		if (!sub_obj)
			continue;

		switch (fields[i].ucl_type) {
		case UCL_INT:
			*(int *)fields[i].value = ucl_object_toint(sub_obj);
			break;

		case UCL_BOOLEAN:
			*(bool *)fields[i].value = ucl_object_toboolean(sub_obj);
			break;

		case UCL_STRING:
			strcpy_s((char *)fields[i].value, ucl_object_tostring(sub_obj), fields[i].size);
			break;

		case UCL_FLOAT:
			*(double *)fields[i].value = ucl_object_todouble(sub_obj);
			break;

		default:
			break;
		}
	}

	return 0;
}

/***********************************************************************************************/

static int parse_nos_options(const ucl_object_t *obj, nereon_nos_option_t *parent_opt);

static struct nos_opt_type {
	enum NEREON_OPT_TYPE type;
	const char *type_str;
} g_nos_opt_types[] = {
	{NEREON_TYPE_INT,     "int"},
	{NEREON_TYPE_BOOL,    "boolean"},
	{NEREON_TYPE_STRING,  "string"},
	{NEREON_TYPE_ARRAY,   "array"},
	{NEREON_TYPE_FLOAT,   "float"},
	{NEREON_TYPE_CONFIG,  "config"},
	{NEREON_TYPE_HELPER,  "helper"},
	{NEREON_TYPE_VERSION, "version"},
	{NEREON_TYPE_UNKNOWN, NULL}
};

/*
 * print NOS option
 */

static void print_nos_option(nereon_nos_option_t *nos_opt, int level)
{
	if (level != 0) {
		char *shift;
		int i;

		/* print self option */
		shift = (char *)malloc(level * sizeof(char) + 1);
		if (!shift)
			return;
		memset(shift, '\t', level * sizeof(char));
		shift[level] = '\0';

		DEBUG_PRINT("%sname: %s\n", shift, nos_opt->name);

		for (i = 0; g_nos_opt_types[i].type_str != NULL; i++) {
			if (g_nos_opt_types[i].type != nos_opt->type)
				continue;
			DEBUG_PRINT("%s  type: %s\n", shift, g_nos_opt_types[i].type_str);
		}

		if (strlen(nos_opt->sw) > 0)
			DEBUG_PRINT("%s  sw: %s\n", shift, nos_opt->sw);

		if (strlen(nos_opt->desc) > 0)
			DEBUG_PRINT("%s  desc: %s\n", shift, nos_opt->desc);

		if (strlen(nos_opt->hint) > 0)
			DEBUG_PRINT("%s  hint: %s\n", shift, nos_opt->hint);

		if (strlen(nos_opt->default_val) > 0)
			DEBUG_PRINT("%s  default_val: %s\n", shift, nos_opt->default_val);

		free(shift);
	}

	/* print sub options */
	if (nos_opt->sub_opts.opts_count > 0) {
		int i;

		for (i = 0; i < nos_opt->sub_opts.opts_count; i++)
			print_nos_option(nos_opt->sub_opts.opts[i], level + 1);
	}
}

/*
 * print NOS schema
 */

static void print_nos_schema(nereon_nos_schema_t *nos_schema)
{
	DEBUG_PRINT("============== NOS program info ==============\n\n");

	DEBUG_PRINT("name : %s\n", nos_schema->prog_info.prog_name);
	DEBUG_PRINT("desc : %s\n", nos_schema->prog_info.desc);
	DEBUG_PRINT("version: %s\n", nos_schema->prog_info.version);
	DEBUG_PRINT("copyright: %s\n", nos_schema->prog_info.copyright);
	DEBUG_PRINT("homepage: %s\n", nos_schema->prog_info.homepage);
	DEBUG_PRINT("license: %s\n", nos_schema->prog_info.license);

	DEBUG_PRINT("============== NOS options ==============\n\n");

	print_nos_option(nos_schema->root_opt, 0);
}

/*
 * free NOS option
 */

static void free_nos_option(nereon_nos_option_t *nos_opt)
{
	if (nos_opt->sub_opts.opts_count > 0) {
		int i;

		for (i = 0; i < nos_opt->sub_opts.opts_count; i++)
			free_nos_option(nos_opt->sub_opts.opts[i]);
		free(nos_opt->sub_opts.opts);
	}

	free(nos_opt);
}

/*
 * add NOS option
 */

static void add_nos_option(nereon_nos_option_t *parent_opt, nereon_nos_option_t *nos_opt)
{
	/* allocate memory for new NOS option */
	if (parent_opt->sub_opts.opts_count == 0)
		parent_opt->sub_opts.opts = (nereon_nos_option_t **)malloc(sizeof(nereon_nos_option_t *));
	else
		parent_opt->sub_opts.opts = (nereon_nos_option_t **)realloc(parent_opt->sub_opts.opts,
				(parent_opt->sub_opts.opts_count + 1) * sizeof(nereon_nos_option_t *));

	if (!parent_opt->sub_opts.opts) {
		parent_opt->sub_opts.opts_count = 0;
		return;
	}

	parent_opt->sub_opts.opts[parent_opt->sub_opts.opts_count++] = nos_opt;
}

/*
 * set NOS option type
 */

int set_nos_option_type(nereon_nos_option_t *nos_opt, const char *opt_type_str)
{
	int i;

	for (i = 0; g_nos_opt_types[i].type_str != NULL; i++) {
		if (strcmp(opt_type_str, g_nos_opt_types[i].type_str) == 0) {
			nos_opt->type = g_nos_opt_types[i].type;
			return 0;
		}
	}

	return -1;
}

/*
 * parse NOS option fields
 */

static int parse_nos_option_fields(const ucl_object_t *obj, nereon_nos_option_t *nos_opt)
{
	char opt_type[NR_MAX_TYPE];
	int i;

	struct ucl_field nos_opt_fields[] = {
		{"type", UCL_STRING, opt_type, sizeof(opt_type)},
		{"switch", UCL_STRING, nos_opt->sw, sizeof(nos_opt->sw)},
		{"desc", UCL_STRING, nos_opt->desc, sizeof(nos_opt->desc)},
		{"hint", UCL_STRING, nos_opt->hint, sizeof(nos_opt->hint)},
		{"default", UCL_STRING, nos_opt->default_val, sizeof(nos_opt->default_val)}
	};

	/* parse options */
	memset(opt_type, 0, sizeof(opt_type));

	if (parse_ucl_fields_ex(obj, nos_opt_fields) != 0)
		return -1;

	/* set NOS option type */
	nos_opt->type = NEREON_TYPE_STRING;
	if (strlen(opt_type) > 0 && set_nos_option_type(nos_opt, opt_type) != 0) {
		DEBUG_PRINT("NOS: Invalid NOS option type '%s'\n", opt_type);
		return -1;
	}

	return 0;
}

/*
 * parse sub options
 */

static int parse_nos_sub_options(const ucl_object_t *obj, nereon_nos_option_t *nos_opt)
{
	const ucl_object_t *sub_obj, *p;
	ucl_object_iter_t it;

	bool requires = false;
	char subopt_type[32];

	int ret = 0;

	struct ucl_field subopt_fields[] = {
		{"requires", UCL_BOOLEAN, &requires, -1},
		{"type", UCL_STRING, subopt_type, sizeof(subopt_type)},
	};

	/* get UCL object for suboptions */
	memset(subopt_type, 0, sizeof(subopt_type));
	sub_obj = ucl_object_lookup(obj, "suboptions");
	if (!sub_obj)
		return 0;

	DEBUG_PRINT("NOS: Parsing suboptions for option '%s'\n", nos_opt->name);

	/* parse suboption fields */
	if (parse_ucl_fields_ex(sub_obj, subopt_fields) != 0) {
		DEBUG_PRINT("NOS: Failed to parse suboptions fields\n");
		return -1;
	}

	/* parse sub options */
	it = ucl_object_iterate_new(sub_obj);
	while ((p = ucl_object_iterate_safe(it, true)) != NULL && ret == 0) {
		if (strcmp(p->key, "option") == 0)
			ret = parse_nos_options(p, nos_opt);
	}
	ucl_object_iterate_free(it);

	if (ret != 0)
		return -1;

	nos_opt->sub_opts.requires = requires;
	if (strlen(subopt_type) == 0 || strcmp(subopt_type, "single") == 0)
		nos_opt->sub_opts.type = NEREON_SUBOPT_SINGLE;
	else if (strcmp(subopt_type, "mixed") == 0)
		nos_opt->sub_opts.type = NEREON_SUBOPT_MIXED;
	else {
		DEBUG_PRINT("NOS: Unknown suboption type '%s'\n", subopt_type);
		return -1;
	}

	return 0;
}

/*
 * parse individual NOS option
 */

static int parse_nos_option(const ucl_object_t *obj, nereon_nos_option_t **nos_opt)
{
	nereon_nos_option_t opt;

	DEBUG_PRINT("NOS: Parsing NOS option '%s'\n", obj->key);

	/* init NOS option */
	memset(&opt, 0, sizeof(nereon_nos_option_t));
	strcpy_s(opt.name, obj->key, sizeof(opt.name));

	/* parse option fields */
	if (parse_nos_option_fields(obj, &opt) != 0)
		return -1;

	/* parse sub options */
	if (parse_nos_sub_options(obj, &opt) != 0)
		return -1;

	/* allocate memory for new NOS option */
	*nos_opt = (nereon_nos_option_t *)malloc(sizeof(nereon_nos_option_t));
	if (*nos_opt == NULL) {
		DEBUG_PRINT("NOS: Out of memory!\n");
		return -1;
	}
	memcpy(*nos_opt, &opt, sizeof(nereon_nos_option_t));

	return 0;
}

/*
 * parse NOS options
 */

static int parse_nos_options(const ucl_object_t *obj, nereon_nos_option_t *parent_opt)
{
	const ucl_object_t *p;
	ucl_object_iter_t it;

	int ret = 0;

	it = ucl_object_iterate_new(obj);
	while ((p = ucl_object_iterate_safe(it, false)) != NULL) {
		nereon_nos_option_t *nos_opt;

		/* parse NOS option */
		if (parse_nos_option(p, &nos_opt) != 0) {
			DEBUG_PRINT("NOS: Invalid NOS option for key '%s'\n", p->key);

			ret = -1;
			break;
		}

		/* add NOS option to list */
		add_nos_option(parent_opt, nos_opt);
	}
	ucl_object_iterate_free(it);

	return ret;
}

/*
 * parse NOS prog info
 */

static int parse_nos_prog_info(const ucl_object_t *obj, nereon_nos_proginfo_t *prog_info)
{
	int i;

	DEBUG_PRINT("NOS: Parsing NOS proginfo\n");

	struct ucl_field prog_info_fields[] = {
		{"name", UCL_STRING, prog_info->prog_name, sizeof(prog_info->prog_name)},
		{"description", UCL_STRING, prog_info->desc, sizeof(prog_info->desc)},
		{"version", UCL_STRING, prog_info->version, sizeof(prog_info->version)},
		{"copyright", UCL_STRING, prog_info->copyright, sizeof(prog_info->copyright)},
		{"homepage", UCL_STRING, prog_info->homepage, sizeof(prog_info->homepage)},
		{"license", UCL_STRING, prog_info->license, sizeof(prog_info->license)},
	};

	return parse_ucl_fields_ex(obj, prog_info_fields);
}

/*
 * parse NOS schema from UCL object
 */

static int parse_nos_schema(const ucl_object_t *obj, nereon_nos_schema_t *nos_schema)
{
	const ucl_object_t *p;
	ucl_object_iter_t it;

	int ret = 0;

	/* initialize NOS schema */
	memset(nos_schema, 0, sizeof(nereon_nos_schema_t));

	it = ucl_object_iterate_new(obj);
	while ((p = ucl_object_iterate_safe(it, true)) != NULL && ret == 0) {
		if (strcmp(p->key, "program") == 0)
			ret = parse_nos_prog_info(p, &nos_schema->prog_info);
		else if (strcmp(p->key, "option") == 0) {
			nos_schema->root_opt = (nereon_nos_option_t *)malloc(sizeof(nereon_nos_option_t));
			if (!nos_schema->root_opt) {
				ret = -1;
				break;
			}
			memset(nos_schema->root_opt, 0, sizeof(nereon_nos_option_t));

			ret = parse_nos_options(p, nos_schema->root_opt);
		}
	}
	ucl_object_iterate_free(it);

	return 0;
}

/*
 * parse NOS info
 */

int nereon_parse_nos_schema(const char *nos_cfg, nereon_nos_schema_t *nos_schema)
{
	struct ucl_parser *parser;
	ucl_object_t *obj = NULL;

	int ret = -1;

	DEBUG_PRINT("NOS: Parsing NOS schema\n");

	/* parse NOS buffer and get UCL object */
	parser = ucl_parser_new(0);
	if (!parser) {
		DEBUG_PRINT("NOS: ucl_parser_new() failed.\n");
		return -1;
	}

	ucl_parser_add_chunk(parser, (const unsigned char *)nos_cfg, strlen(nos_cfg));
	if (ucl_parser_get_error(parser) != NULL ||
		(obj = ucl_parser_get_object(parser)) == NULL) {
		DEBUG_PRINT("NOS: Failed to parse NOS buffer(err:%s)\n", ucl_parser_get_error(parser));
		goto end;
	}

	/* parse NOS info from UCL object */
	ret = parse_nos_schema(obj, nos_schema);

end:
	if (parser)
		ucl_parser_free(parser);

	if (obj)
		ucl_object_unref(obj);

	return ret;
}

/*
 * free NOS options
 */

void nereon_free_nos_schema(nereon_nos_schema_t *nos_schema)
{
	free_nos_option(&nos_schema->root_opt);
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	FILE *fp;
	char nos_cfg[BUF_SIZE];
	size_t len;

	int ret = -1;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <NOS configuration file>\n", argv[0]);
		exit(1);
	}

	/* read file contents */
	fp = fopen(argv[1], "r");
	if (!fp) {
		fprintf(stderr, "Could not open file '%s' for reading\n", argv[1]);
		exit(1);
	}

	len = fread(nos_cfg, 1, BUF_SIZE - 1, fp);
	if (len > 0) {
		nereon_nos_schema_t nos_schema;

		nos_cfg[len] = '\0';
		ret = nereon_parse_nos_schema(nos_cfg, &nos_schema);
		if (ret == 0) {
			print_nos_schema(&nos_schema);
			nereon_free_nos_schema(&nos_schema);
		}
	}
	fclose(fp);

	return ret;
}

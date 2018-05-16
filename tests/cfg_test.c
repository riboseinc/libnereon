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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <ucl.h>

#include "mconfig.h"
#include "test_util.h"

#define TEST_CFG_HCL                  "tests/cfg.hcl"

struct hcl_option {
	char key[128];
	int type;

	int level;

	struct hcl_option *childs;
	struct hcl_option *next;
	struct hcl_option *parent;

	void *data;
}

static struct hcl_option *g_opts;

static void usage(const char *prog_name)
{
	fprintf(stderr, "%s <HCL filename>\n", prog_name);
	exit(1);
}

void add_hcl_option(const char *key, int type, int level, const char *parent_key)
{
	struct hcl_option *opt;

	opt = (struct hcl_option *)malloc(sizeof(struct hcl_option));
	if (!opt)
		return;

	memset(opt, 0, sizeof(struct hcl_option));

	strlcpy(opt->key, key);
	strlcpy(opt->parent_key, parent_key);
	opt->type = type;
	opt->level = level;

	if (opt->level == 0) {

	}
}

void
ucl_obj_dump (const ucl_object_t *obj, unsigned int shift)
{
	int num = shift * 4 + 5;
	char *pre = (char *) malloc (num * sizeof(char));
	const ucl_object_t *cur, *tmp;
	ucl_object_iter_t it = NULL, it_obj = NULL;

	pre[--num] = 0x00;
	while (num--)
		pre[num] = 0x20;

	tmp = obj;

	while ((obj = ucl_object_iterate (tmp, &it, false))) {
		printf ("%sucl object address: %p\n", pre + 4, obj);
		if (obj->key != NULL) {
			printf ("%skey: \"%s\"\n", pre, ucl_object_key (obj));
		}
		printf ("%sref: %u\n", pre, obj->ref);
		printf ("%slen: %u\n", pre, obj->len);
		printf ("%sprev: %p\n", pre, obj->prev);
		printf ("%snext: %p\n", pre, obj->next);
		if (obj->type == UCL_OBJECT) {
			printf ("%stype: UCL_OBJECT\n", pre);
			printf ("%svalue: %p\n", pre, obj->value.ov);
			it_obj = NULL;
			while ((cur = ucl_object_iterate (obj, &it_obj, true))) {
				ucl_obj_dump (cur, shift + 2);
			}
		}
		else if (obj->type == UCL_ARRAY) {
			printf ("%stype: UCL_ARRAY\n", pre);
			printf ("%svalue: %p\n", pre, obj->value.av);
			it_obj = NULL;
			while ((cur = ucl_object_iterate (obj, &it_obj, true))) {
				ucl_obj_dump (cur, shift + 2);
			}
		}
		else if (obj->type == UCL_INT) {
			printf ("%stype: UCL_INT\n", pre);
			printf ("%svalue: %jd\n", pre, (intmax_t)ucl_object_toint (obj));
		}
		else if (obj->type == UCL_FLOAT) {
			printf ("%stype: UCL_FLOAT\n", pre);
			printf ("%svalue: %f\n", pre, ucl_object_todouble (obj));
		}
		else if (obj->type == UCL_STRING) {
			printf ("%stype: UCL_STRING\n", pre);
			printf ("%svalue: \"%s\"\n", pre, ucl_object_tostring (obj));
		}
		else if (obj->type == UCL_BOOLEAN) {
			printf ("%stype: UCL_BOOLEAN\n", pre);
			printf ("%svalue: %s\n", pre, ucl_object_tostring_forced (obj));
		}
		else if (obj->type == UCL_TIME) {
			printf ("%stype: UCL_TIME\n", pre);
			printf ("%svalue: %f\n", pre, ucl_object_todouble (obj));
		}
		else if (obj->type == UCL_USERDATA) {
			printf ("%stype: UCL_USERDATA\n", pre);
			printf ("%svalue: %p\n", pre, obj->value.ud);
		}
	}

	free (pre);
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	struct ucl_parser *parser;
	ucl_object_t *obj = NULL;

	char *buf = NULL;
	size_t buf_size;

	int ret = -1;

	if (argc != 2) {
		usage(argv[0]);
	}

	if ((buf_size = read_file_contents(argv[1], &buf)) <= 0) {
		exit(1);
	}

	parser = ucl_parser_new(0);
	if (!parser) {
		fprintf(stderr, "ucl_parser_new() failed.\n");
		goto end;
	}

	ucl_parser_add_chunk(parser, (const unsigned char *)buf, buf_size);
	if (ucl_parser_get_error(parser) != NULL) {
		fprintf(stderr, "Failed to parse HCL contents(err:%s)\n", ucl_parser_get_error(parser));
		goto end;
	}

	obj = ucl_parser_get_object(parser);
	if (!obj) {
		fprintf(stderr, "Failed to get UCL object(err:%s)\n", ucl_parser_get_error(parser));
		goto end;
	}
	ucl_obj_dump(obj, 0);

	/* build HCL options from object */
	build_hcl_cfg(obj);

	ret = 0;

end:
	if (parser)
		ucl_parser_free(parser);

	if (obj)
		ucl_object_unref(obj);

	if (buf)
		free(buf);

	return ret;
}

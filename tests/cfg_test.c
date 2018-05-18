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

struct hcl_config {
	char key[128];
	int type;

	int level;

	struct hcl_config *childs;
	struct hcl_config *next;

	void *data;
};

static struct hcl_config *g_cfg_list;

void free_hcl_configs(struct hcl_config *cfg)
{
	if (!cfg)
		return;

	if (cfg->childs) {
		struct hcl_config *p = cfg->childs;

		while (p) {
			free_hcl_configs(p);
			p = p->next;
		}
	}
	free(cfg);
}

static void print_hcl_configs(struct hcl_config *cfg)
{
	if (!cfg)
		return;

	fprintf(stderr, "key: %s\n", cfg->key);

	if (cfg->childs) {
		struct hcl_config *p = cfg->childs;

		while (p) {
			print_hcl_configs(p);
			p = p->next;
		}
	}
}

static struct hcl_config *add_cfg_to_list(struct hcl_config *parent_cfg)
{
	struct hcl_config *cfg;

	cfg = (struct hcl_config *)malloc(sizeof(struct hcl_config));
	if (!cfg)
		return NULL;
	memset(cfg, 0, sizeof(struct hcl_config));

	if (!parent_cfg) {
		g_cfg_list = cfg;
		return cfg;
	}

	if (!parent_cfg->childs) {
		parent_cfg->childs = cfg;
	} else {
		struct hcl_config *p = parent_cfg->childs;

		while (p->next)
			p = p->next;

		p->next = cfg;
	}

	return cfg;
}

void
ucl_obj_dump(const ucl_object_t *obj, struct hcl_config *parent_cfg, unsigned int shift, int level)
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
		struct hcl_config *cfg;

		/* create HCL config from UCL object */
		cfg = add_cfg_to_list(parent_cfg);
		if (!cfg) {
			fprintf(stderr, "Out of memory!\n");
			return;
		}

		printf ("%sucl object address: %p\n", pre + 4, obj);
		if (obj->key != NULL) {
			printf ("%skey: \"%s\"\n", pre, ucl_object_key (obj));
			strlcpy(cfg->key, obj->key, sizeof(cfg->key));
		}
		printf ("%slevel: %u\n", pre, level);
		printf ("%scfg: %p\n", pre, cfg);
		printf ("%sparent_cfg: %p\n", pre, parent_cfg);
		printf ("%sref: %u\n", pre, obj->ref);
		printf ("%slen: %u\n", pre, obj->len);
		printf ("%sprev: %p\n", pre, obj->prev);
		printf ("%snext: %p\n", pre, obj->next);
		if (obj->type == UCL_OBJECT) {
			printf ("%stype: UCL_OBJECT\n", pre);
			printf ("%svalue: %p\n", pre, obj->value.ov);
			it_obj = NULL;
			while ((cur = ucl_object_iterate (obj, &it_obj, true))) {
				ucl_obj_dump (cur, cfg, shift + 2, level + 1);
			}
		}
		else if (obj->type == UCL_ARRAY) {
			printf ("%stype: UCL_ARRAY\n", pre);
			printf ("%svalue: %p\n", pre, obj->value.av);
			it_obj = NULL;
			while ((cur = ucl_object_iterate (obj, &it_obj, true))) {
				ucl_obj_dump (cur, cfg, shift + 2, level + 1);
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

		/* create config object */

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

	if ((buf_size = read_file_contents(TEST_CFG_HCL, &buf)) <= 0) {
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

	/* init config list */
	ucl_obj_dump(obj, NULL, 0, 0);

	/* print hcl configs */
	print_hcl_configs(g_cfg_list);

	ret = 0;

end:
	if (parser)
		ucl_parser_free(parser);

	if (obj)
		ucl_object_unref(obj);

	if (buf)
		free(buf);

	free_hcl_configs(g_cfg_list);

	return ret;
}

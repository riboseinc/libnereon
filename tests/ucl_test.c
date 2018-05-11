#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <ucl.h>

static void usage(const char *prog_name)
{
	fprintf(stderr, "%s <HCL filename>\n", prog_name);
	exit(1);
}

/*
 * read file contents
 */

static size_t read_file_contents(const char *fpath, char **buf)
{
	FILE *fp;
	struct stat st;
	size_t read_len;

	if (stat(fpath, &st) != 0 || !S_ISREG(st.st_mode) || st.st_size == 0)
		return -1;

	fp = fopen(fpath, "r");
	if (!fp) {
		fprintf(stderr, "Could not open file '%s' for reading\n", fpath);
		return -1;
	}

	*buf = (char *) malloc(st.st_size + 1);
	if (*buf == NULL) {
		fprintf(stderr, "Out of memory!\n");
		fclose(fp);
		return -1;
	}

	read_len = fread(*buf, 1, st.st_size, fp);
	if (read_len > 0)
		(*buf)[read_len] = '\0';

	fclose(fp);

	return read_len;
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
	ucl_parser_set_filevars(parser, argv[1], true);

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

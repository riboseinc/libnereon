
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

#if HAVE_CONFIG_H
#include "../config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <ucl.h>

/*
 * print usage
 */

static void print_usage(int exit_code)
{
	fprintf(stderr, "Usage: json2noc <options>\n"
					"  -i <JSON file>       Specify JSON configuration file to be converted\n"
					"  -o <output NOC file> Specify output NOC file path (optional)\n"
					"  -h                   Print help message\n");

	exit(exit_code);
}

/*
 * open file in safe writting mode
 */

FILE *fopen_wsafe(const char *fpath)
{
	int fd;
	FILE *fp;

	remove(fpath);

	fd = open(fpath, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if (fd < 0)
		return NULL;

	fp = fdopen(fd, "w");
	if (!fp)
		close(fd);

	return fp;
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
		return 0;

	fp = fopen(fpath, "r");
	if (!fp)
		return 0;

	*buf = (char *) malloc(st.st_size + 1);
	if (*buf == NULL) {
		fclose(fp);
		return 0;
	}

	read_len = fread(*buf, 1, st.st_size, fp);
	if (read_len > 0)
		(*buf)[read_len] = '\0';

	fclose(fp);

	return read_len;
}

/*
 * get NOC path
 */

static int get_noc_path(const char *json_fpath, char **noc_fpath)
{
	char *p, *ret;
	size_t ret_len;

	p = strrchr(json_fpath, '.');
	if (!p)
		ret_len = strlen(json_fpath) - strlen(p) + 4;
	else
		ret_len = strlen(json_fpath) + 4;

	ret = (char *)malloc(ret_len + 1);
	if (!ret) {
		fprintf(stderr, "Out of memory!\n");
		return -1;
	}

	memset(ret, 0, ret_len + 1);
	if (p)
		strncpy(ret, json_fpath, strlen(json_fpath) - strlen(p));
	else
		strcpy(ret, json_fpath);
	strcat(ret, ".noc");

	*noc_fpath = ret;

	return 0;
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	const char *json_fpath = NULL;

	FILE *noc_fp;
	char *noc_fpath = NULL;
	unsigned char *ucl_str;

	char *json_str = NULL;
	size_t json_str_len;

	struct ucl_parser *parser = NULL;
	ucl_object_t *u_obj = NULL;

	int ret = -1;

	if (argc > 1) {
		int opt;
		while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
			switch (opt) {
			case 'i':
				json_fpath = optarg;
				break;

			case 'o':
				noc_fpath = strdup(optarg);
				break;

			case 'h':
				print_usage(0);
				break;

			default:
				print_usage(1);
				break;
			}
		}
	}

	if (!json_fpath) {
		fprintf(stderr, "Please specify JSON configuration file to be converted\n");
		print_usage(1);
	}

	if (!noc_fpath && get_noc_path(json_fpath, &noc_fpath) != 0) {
		fprintf(stderr, "Could not get NOC configuration path");
		exit(1);
	}

	noc_fp = fopen_wsafe(noc_fpath);
	if (!noc_fp) {
		fprintf(stderr, "Could not open NOC configuration file '%s' for writing mode", noc_fpath);
		free(noc_fpath);
		exit(1);
	}
	free(noc_fpath);

	/* get JSON data */
	if ((json_str_len = read_file_contents(json_fpath, &json_str)) <= 0) {
		fprintf(stderr, "Could not read file contents from '%s'\n", json_fpath);
		goto end;
	}

	/* get UCL object from JSON data */
	parser = ucl_parser_new(0);
	if (!parser) {
		fprintf(stderr, "Could not get UCL parser\n");
		goto end;
	}

	ucl_parser_add_chunk(parser, (const unsigned char *)json_str, json_str_len);
	if (ucl_parser_get_error(parser) != NULL ||
		((u_obj = ucl_parser_get_object(parser)) == NULL)) {
		fprintf(stderr, "Failed to parse JSON data(err:%s)\n", ucl_parser_get_error(parser));
		goto end;
	}

	ucl_str = ucl_object_emit(u_obj, UCL_EMIT_CONFIG);
	if (ucl_str) {
		fprintf(noc_fp, "%s", (char *)ucl_str);
		free(ucl_str);
	}

	ret = 0;

end:
	fclose(noc_fp);

	if (json_str)
		free(json_str);

	if (parser)
		ucl_parser_free(parser);

	if (u_obj)
		ucl_object_unref(u_obj);

	return ret;
}

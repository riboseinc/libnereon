
#if HAVE_CONFIG_H
#include "../config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "nereon.h"

/*
 * read file contents
 */

static size_t read_file_contents(const char *fpath, char **buf)
{
	FILE *fp;
	struct stat st;
	size_t read_len;

	if (stat(fpath, &st) != 0 || !S_ISREG(st.st_mode) || st.st_size == 0) {
		fprintf(stderr, "Could not get stat of file '%s'\n", fpath);
		return 0;
	}

	fp = fopen(fpath, "r");
	if (!fp) {
		fprintf(stderr, "Could not open file '%s' for reading\n", fpath);
		return 0;
	}

	*buf = (char *) malloc(st.st_size + 1);
	if (*buf == NULL) {
		fprintf(stderr, "Out of memory!\n");
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
 * write source and header file
 */

static int write_cc_files(const char *src_fpath, const char *hdr_fpath, const char *nos_str)
{
	FILE *src_fp, *hdr_fp;
	const char *hdr_fname, *p;

	/* get header file name */
	p = strrchr(hdr_fpath, '/');
	if (p)
		hdr_fname = p + 1;
	else
		hdr_fname = hdr_fpath;

	/* create source file */
	src_fp = fopen_wsafe(src_fpath);
	if (!src_fp)
		return -1;

	fprintf(src_fp, "#include \"%s\"\n\n", hdr_fname);
	fprintf(src_fp, "static const char *g_nos_cfg = \"");

	p = nos_str;
	while (*p != '\0') {
		if (*p == '\n')
			fprintf(src_fp, "\\n");
		else {
			if (*p == '"')
				fputc('\\', src_fp);

			fputc(*p, src_fp);
		}
		p++;
	}
	fprintf(src_fp, "\";\n\n");
	fprintf(src_fp, "const char *get_nos_cfg(void)\n{\n\treturn g_nos_cfg;\n}\n");

	fclose(src_fp);

	/* create header file */
	hdr_fp = fopen_wsafe(hdr_fpath);
	if (!hdr_fp) {
		remove(src_fpath);
		return -1;
	}

	fprintf(hdr_fp, "\nconst char *get_nos_cfg(void);\n");

	fclose(hdr_fp);

	return 0;
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	nereon_ctx_t ctx;

	char *nos_base_name;

	char cc_src_fpath[256], cc_hdr_fpath[256];
	char *nos_str;

	int ret;

	/* check argument */
	if (argc != 3) {
		fprintf(stderr, "Usage: nos_to_cc <NOS configuration file> <NOS CC output path>\n");
		exit(1);
	}

	/* read NOS configuration from given file */
	if (read_file_contents(argv[1], &nos_str) <= 0)	{
		fprintf(stderr, "Could not read NOS configuration from '%s'\n", argv[1]);
		exit(1);
	}

	/* check NOS configuration has valid syntax */
	memset(&ctx, 0, sizeof(nereon_ctx_t));
	if (nereon_ctx_init(&ctx, nos_str) != 0) {
		fprintf(stderr, "Invalid NOS configuration(err:%s)", nereon_get_errmsg());

		free(nos_str);
		exit(1);
	}
	nereon_ctx_finalize(&ctx);

	/* get basename of NOS configuration path */
	nos_base_name = strrchr(argv[1], '/');
	if (!nos_base_name)
		nos_base_name = argv[1];
	else
		nos_base_name++;

	/* write source and header file */
	snprintf(cc_src_fpath, sizeof(cc_src_fpath), "%s%s.c", argv[2], nos_base_name);
	snprintf(cc_hdr_fpath, sizeof(cc_hdr_fpath), "%s%s.h", argv[2], nos_base_name);

	ret = write_cc_files(cc_src_fpath, cc_hdr_fpath, nos_str);
	free(nos_str);

	if (ret != 0) {
		fprintf(stderr, "Could not generate %s and %s files\n", cc_src_fpath, cc_hdr_fpath);
		exit(1);
	}

	return 0;
}

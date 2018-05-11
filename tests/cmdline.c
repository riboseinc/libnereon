#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>

#include "mconfig.h"

#define CMDLINE_HCL_FILE                    "cmdline.hcl"

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

/*
 * main function
 */

int main(int argc, char *argv[])
{
	char *hcl_options;

	/* read HCL options */
	if (read_file_contents(CMDLINE_HCL_FILE, &hcl_options) <= 0)
		exit(1);

	
}
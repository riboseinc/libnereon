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

#include "test_util.h"
#include "mconfig.h"

#define TEST_CMDLINE_HCL                 "tests/cmdline.hcl"
#define TEST_CONFIG_HCL                  "tests/cfg.hcl"

/*
 * main function
 */

int main(int argc, char *argv[])
{
	mconfig_ctx_t mctx;

	char *hcl_options = NULL;
	int ret;

	/* read HCL options */
	if (read_file_contents(TEST_CMDLINE_HCL, &hcl_options) <= 0)
		exit(1);

	/* initialize multiconfig context */
	ret = mconfig_ctx_init(&mctx, hcl_options);
	if (ret != 0) {
		fprintf(stderr, "Could not initialize multiconfig context(err:%s)\n", mconfig_get_errmsg());
		free(hcl_options);

		return -1;
	}
	free(hcl_options);

	/* print command line usage */
	if (mconfig_parse_config(&mctx, TEST_CONFIG_HCL) != 0) {
		fprintf(stderr, "Failed to parse config file(err:%s)\n", mconfig_get_errmsg());
	}

	/* finalize multiconfig context */
	mconfig_ctx_finalize(&mctx);

	return ret;
}

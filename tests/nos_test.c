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

#include <sys/stat.h>

#include "nereon.h"
#include "rvc.nos.h"

/*
 * main function
 */

int main(int argc, char *argv[])
{
	nereon_ctx_t *ctx;
	int ret;
	bool require_exit = false;

	/* initialize nereon context */
	ctx = nereon_ctx_init(get_rvc_nos_cfg());
	if (!ctx) {
		fprintf(stderr, "Could not initialize nereon context(err:%s)\n", nereon_get_errmsg());
		return -1;
	}

	/* parse command line */
	ret = nereon_parse_cmdline(ctx, argc, argv, &require_exit);
	if (ret == 0) {

	}

#if 0
	/* print command line usage */
	ret = nereon_parse_cmdline(&ctx, argc, argv, &require_exit);
	if (ret != 0 || require_exit) {
		if (ret != 0)
			fprintf(stderr, "Failed to parse command line(err:%s)\n", nereon_get_errmsg());

		nereon_print_usage(&ctx);
	}
#endif

	/* finalize nereon context */
	nereon_ctx_finalize(ctx);

	return ret;
}

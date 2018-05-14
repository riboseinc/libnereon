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

#include "common.h"

#include "hcl.h"
#include "cmdline.h"
#include "err.h"
#include "mconfig.h"

/*
 * intiialize multi-config
 */

int mconfig_init(const char *hcl_options, int argc, char **argv)
{
	struct mconfig_hcl_options *mcfg_opts = NULL;
	int mcfg_opts_count = 0;

	DEBUG_PRINT("Initializing multiconfig\n");

	/* parse HCL options */
	if (mconfig_parse_hcl_options(hcl_options, &mcfg_opts, &mcfg_opts_count) != 0) {
		DEBUG_PRINT("Failed to parse HCL options(err:%s)\n", mconfig_get_err());
		return -1;
	}

	DEBUG_PRINT("Success to parse HCL options\n");

	if (mconfig_parse_cmdline(mcfg_opts, mcfg_opts_count, argc, argv) != 0) {
		free(mcfg_opts);
		return -1;
	}
	free(mcfg_opts);

	return 0;
}

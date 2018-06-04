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

#include "rvd.nos.h"
#include "nereon.h"

#define NOC_CONFIG_FILE                     "tests/rvd.noc"

struct rvd_options {
	char *config_fpath;
	bool go_daemon;

	bool check_config;
	bool print_version;
	bool print_help;

	char *ovpn_bin_path;
	bool ovpn_root_check;
	bool ovpn_use_scripts;

	int allowed_uid;
	bool restrict_cmd_sock;

	char *log_dir_path;
	char *vpn_config_dir;
};

/*
 * main function
 */

int main(int argc, char *argv[])
{
	nereon_ctx_t ctx;
	int ret;

	struct rvd_options rvd_opts;

	struct nereon_config_option cfg_opts[] = {
		{"config_file", NEREON_TYPE_STRING, &rvd_opts.config_fpath},
		{"go_daemon", NEREON_TYPE_BOOL, &rvd_opts.go_daemon},
		{"check_config", NEREON_TYPE_BOOL, &rvd_opts.check_config},
		{"print_version", NEREON_TYPE_BOOL, &rvd_opts.print_version},
		{"helper", NEREON_TYPE_BOOL, &rvd_opts.print_help},
		{"openvpn_bin", NEREON_TYPE_STRING, &rvd_opts.ovpn_bin_path},
		{"openvpn_root_check", NEREON_TYPE_BOOL, &rvd_opts.ovpn_root_check},
		{"openvpn_updown_scripts", NEREON_TYPE_BOOL, &rvd_opts.ovpn_use_scripts},
		{"user_id", NEREON_TYPE_INT, &rvd_opts.allowed_uid},
		{"restrict_socket", NEREON_TYPE_BOOL, &rvd_opts.restrict_cmd_sock},
		{"log_directory", NEREON_TYPE_STRING, &rvd_opts.log_dir_path},
		{"vpn_config_paths", NEREON_TYPE_STRING, &rvd_opts.vpn_config_dir}
	};

	/* initialize nereon context */
	ret = nereon_ctx_init(&ctx, get_nos_cfg());
	if (ret != 0) {
		fprintf(stderr, "Could not initialize nereon context(err:%s)\n", nereon_get_errmsg());
		exit(1);
	}

	memset(&rvd_opts, 0, sizeof(struct rvd_options));
	if (nereon_parse_config_file(&ctx, NOC_CONFIG_FILE) != 0) {
		fprintf(stderr, "Could not parse NOC configuration(err:%s)\n", nereon_get_errmsg());
	} else if (nereon_get_config_options(&ctx, cfg_opts, sizeof(cfg_opts) / sizeof(struct nereon_config_option)) != 0) {
		fprintf(stderr, "Could not get configuration optinos(err:%s)\n", nereon_get_errmsg());
	}

	/* finalize nereon context */
	nereon_ctx_finalize(&ctx);

	return ret;
}

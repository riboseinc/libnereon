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

#include "rvd.nos.h"
#include "nereon.h"

#define NOC_CONFIG_FILE                     "rvd.noc"

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
 * print rvd config options
 */

static void print_config_options(struct rvd_options *opt)
{
	fprintf(stderr, "config_fpath => %s\n", opt->config_fpath);
	fprintf(stderr, "go_daemon => %d\n", opt->go_daemon);
	fprintf(stderr, "check_config => %d\n", opt->check_config);
	fprintf(stderr, "print_version => %d\n", opt->print_version);
	fprintf(stderr, "print_help => %d\n", opt->print_help);
	fprintf(stderr, "ovpn_bin_path => %s\n", opt->ovpn_bin_path);
	fprintf(stderr, "ovpn_root_check => %d\n", opt->ovpn_root_check);
	fprintf(stderr, "ovpn_use_scripts => %d\n", opt->ovpn_use_scripts);
	fprintf(stderr, "allowed_uid => %d\n", opt->allowed_uid);
	fprintf(stderr, "restrict_cmd_sock => %d\n", opt->restrict_cmd_sock);
	fprintf(stderr, "log_dir_path => %s\n", opt->log_dir_path);
	fprintf(stderr, "vpn_config_dir => %s\n", opt->vpn_config_dir);
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	nereon_ctx_t ctx;
	int ret;

	struct rvd_options rvd_opts;

	struct nereon_config_option cfg_opts[] = {
		{"config_file", NEREON_TYPE_CONFIG, false, &rvd_opts.config_fpath},
		{"go_daemon", NEREON_TYPE_BOOL, false, &rvd_opts.go_daemon},
		{"check_config", NEREON_TYPE_BOOL, false, &rvd_opts.check_config},
		{"print_version", NEREON_TYPE_BOOL, false, &rvd_opts.print_version},
		{"helper", NEREON_TYPE_BOOL, false, &rvd_opts.print_help},
		{"openvpn_bin", NEREON_TYPE_STRING, true, &rvd_opts.ovpn_bin_path},
		{"openvpn_root_check", NEREON_TYPE_BOOL, false, &rvd_opts.ovpn_root_check},
		{"openvpn_updown_scripts", NEREON_TYPE_BOOL, false, &rvd_opts.ovpn_use_scripts},
		{"user_id", NEREON_TYPE_INT, true, &rvd_opts.allowed_uid},
		{"restrict_socket", NEREON_TYPE_BOOL, false, &rvd_opts.restrict_cmd_sock},
		{"log_directory", NEREON_TYPE_STRING, true, &rvd_opts.log_dir_path},
		{"vpn_config_paths", NEREON_TYPE_STRING, true, &rvd_opts.vpn_config_dir}
	};

	int rvd_opts_count;

	/* initialize nereon context */
	ret = nereon_ctx_init(&ctx, get_nos_cfg());
	if (ret != 0) {
		fprintf(stderr, "Could not initialize nereon context(err:%s)\n", nereon_get_errmsg());
		exit(1);
	}

	/* print command line usage */
	if (nereon_parse_cmdline(&ctx, argc, argv) != 0) {
		fprintf(stderr, "Failed to parse command line(err:%s)\n", nereon_get_errmsg());
		nereon_print_usage(&ctx);

		nereon_ctx_finalize(&ctx);

		exit(1);
	}

	memset(&rvd_opts, 0, sizeof(struct rvd_options));
	if (nereon_parse_config_file(&ctx, NOC_CONFIG_FILE) != 0) {
		fprintf(stderr, "Could not parse NOC configuration(err:%s)\n", nereon_get_errmsg());
		nereon_ctx_finalize(&ctx);

		exit(1);
	}

	rvd_opts_count = sizeof(cfg_opts) / sizeof(struct nereon_config_option);
	ret = nereon_get_config_options(&ctx, cfg_opts, rvd_opts_count);
	if (ret == 0) {
		print_config_options(&rvd_opts);
	} else {
		fprintf(stderr, "Failed to get configuration options(err:%s)\n", nereon_get_errmsg());
	}

	/* finalize nereon context */
	nereon_ctx_finalize(&ctx);

	return ret;
}

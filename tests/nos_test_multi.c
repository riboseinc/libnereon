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

struct rvc_options {
	char *conn_name, *disconn_name, *reconn_name, *status_name;

	bool connect, disconnect;
	bool reconnect, status;

	bool json;

	bool edit;
	bool edit_auto_connect, edit_pre_exec_cmd, edit_profile;
	char *edit_data;

	bool remove, remove_force;

	bool import, import_tblk, import_ovpn;
	char *import_path;

	bool reload;

	bool dns_override;
	bool dns_override_enable, dns_override_disable, dsn_override_status;
	char *dns_srvs;

	bool script_security;
	bool script_security_enable, script_security_disable;

	bool version;
	bool helper;
};

/*
 * print RVC options
 */

static void print_options(struct rvc_options *opt)
{
	fprintf(stdout, "=============== Output of RVC options =================\n");

	if (opt->connect)
		fprintf(stdout, "connect '%s'\n", opt->conn_name);
	if (opt->disconnect)
		fprintf(stdout, "disconnect '%s'\n", opt->disconn_name);
	if (opt->reconnect)
		fprintf(stdout, "reconnect '%s'\n", opt->reconn_name);
	if (opt->status)
		fprintf(stdout, "status '%s'\n", opt->status_name);

	if (opt->json)
		fprintf(stdout, "json enabled\n");

	if (opt->edit) {
		if (opt->edit_auto_connect)
			fprintf(stdout, "edit '%s' --auto-connect '%s'\n",
					opt->conn_name, opt->edit_auto_connect ? "true" : "false");

		if (opt->edit_pre_exec_cmd)
			fprintf(stdout, "edit '%s' --pre-exec-cmd '%s'\n", opt->conn_name, opt->edit_data);

		if (opt->edit_profile)
			fprintf(stdout, "edit '%s' --profile '%s'\n", opt->conn_name, opt->edit_data);
	}

	if (opt->remove) {
		if (opt->remove_force)
			fprintf(stdout, "--remove '%s' --force\n", opt->conn_name);
		else
			fprintf(stdout, "--remove '%s'\n", opt->conn_name);
	}

	if (opt->import) {
		if (opt->import_tblk)
			fprintf(stdout, "import --import-tblk '%s'\n", opt->import_path);
		if (opt->import_ovpn)
			fprintf(stdout, "import --import-ovpn '%s'\n", opt->import_path);
	}

	if (opt->reload)
		fprintf(stdout, "reload\n");

	if (opt->dns_override) {
		if (opt->dns_override_enable)
			fprintf(stdout, "dns-override --enable '%s'\n", opt->dns_srvs);
		if (opt->dns_override_disable)
			fprintf(stdout, "dns-override --disable");
		if (opt->dsn_override_status)
			fprintf(stdout, "dns-override --status");
	}

	if (opt->script_security) {
		if (opt->script_security_enable)
			fprintf(stdout, "script-security --enable\n");
		if (opt->script_security_disable)
			fprintf(stdout, "script-security --disable\n");
	}

	if (opt->version)
		fprintf(stdout, "version\n");

	if (opt->helper)
		fprintf(stdout, "help\n");
}

/*
 * main function
 */

int main(int argc, char *argv[])
{
	nereon_ctx_t ctx;
	struct rvc_options opt;

	int ret;
	bool require_exit = false;

	struct nereon_config_option rvc_opts[] = {
		{"connect", NEREON_TYPE_STRING, false, &opt.connect, &opt.conn_name},
		{"disconnect", NEREON_TYPE_STRING, false, &opt.disconnect, &opt.disconn_name},
		{"reconnect", NEREON_TYPE_STRING, false, &opt.reconnect, &opt.reconn_name},
		{"status", NEREON_TYPE_STRING, false, &opt.status, &opt.status_name},

		{"json", NEREON_TYPE_BOOL, false, NULL, &opt.json},

		{"edit", NEREON_TYPE_STRING, false, &opt.edit, &opt.conn_name},
		{"edit.auto-connect", NEREON_TYPE_STRING, false, &opt.edit_auto_connect, &opt.edit_data},
		{"edit.pre-exec-cmd", NEREON_TYPE_STRING, false, &opt.edit_pre_exec_cmd, &opt.edit_data},
		{"edit.profile", NEREON_TYPE_STRING, false, &opt.edit_profile, &opt.edit_data},

		{"remove", NEREON_TYPE_STRING, false, &opt.remove, &opt.conn_name},
		{"remove.force", NEREON_TYPE_BOOL, false, NULL, &opt.remove_force},

		{"import", NEREON_TYPE_BOOL, false, NULL, &opt.import},
		{"import.new-from-tblk", NEREON_TYPE_STRING, false, &opt.import_tblk, &opt.import_path},
		{"import.new-from-ovpn", NEREON_TYPE_STRING, false, &opt.import_ovpn, &opt.import_path},

		{"reload", NEREON_TYPE_STRING, false, &opt.reload, &opt.reload},

		{"dns-override", NEREON_TYPE_BOOL, false, NULL, &opt.dns_override},
		{"dns-override.enable", NEREON_TYPE_STRING, false, &opt.dns_override_enable, &opt.dns_srvs},
		{"dns-override.disable", NEREON_TYPE_BOOL, false, NULL, &opt.dns_override_disable},
		{"dns-override.status", NEREON_TYPE_BOOL, false, NULL, &opt.dsn_override_status},

		{"script-security", NEREON_TYPE_BOOL, false, NULL, &opt.script_security},
		{"script-security.enable", NEREON_TYPE_BOOL, false, NULL, &opt.script_security_enable},
		{"script-security.disable", NEREON_TYPE_BOOL, false, NULL, &opt.script_security_disable},

		{"version", NEREON_TYPE_BOOL, false, NULL, &opt.version},
		{"help", NEREON_TYPE_BOOL, false, NULL, &opt.helper}
	};

	/* initialize nereon context */
	ret = nereon_ctx_init(&ctx, get_rvc_nos_cfg());
	if (ret != 0) {
		fprintf(stderr, "Could not initialize nereon context(err:%s)\n", nereon_get_errmsg());
		return -1;
	}

	fprintf(stderr, "libnereon version: '%s'\n", nereon_get_version_info());

	/* print command line usage */
	ret = nereon_parse_cmdline(&ctx, argc, argv, &require_exit);
	if (ret != 0 || require_exit) {
		if (ret != 0)
			fprintf(stderr, "Failed to parse command line(err:%s)\n", nereon_get_errmsg());

		nereon_print_usage(&ctx);
		goto end;
	}

	/* get configuration options */
	memset(&opt, 0, sizeof(struct rvc_options));
	if (nereon_get_config_options(&ctx, rvc_opts, sizeof(rvc_opts) / sizeof(struct nereon_config_option)) != 0) {
		fprintf(stderr, "Could not get confiugration options(err:%s)\n", nereon_get_errmsg());
	} else {
		print_options(&opt);
	}

end:
	/* finalize nereon context */
	nereon_ctx_finalize(&ctx);

	return ret;
}

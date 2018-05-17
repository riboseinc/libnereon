
# This HCL contains the configurations related to command line options

log_directory {
	type = "string"
	cmdline "switch" {
		short = "l"
		long = "log-dir"
	}

	cmdline "description" {
		short = "log directory"
		long = "Specify the path of log directory"
	}

	env = "KAOHI_LOG_DIR"
	config = "global.log_directory"
}

log_level {
	type = "int"
	cmdline "switch" {
		short = "d"
		long = "verbose"
	}

	cmdline "description" {
		short = "verbose level (0 ~ 3)"
		long = "Specify the verbose level"
	}

	env = "KAOHI_LOG_LEVEL"
	config = "global.log_level"
}

listen_address {
	type = "ipport"
	cmdline "switch" {
		short = "L"
		long = "listen"
	}

	cmdline "description" {
		short = "IP:port"
		long = "Specify the listening address for Kaohi console"
	}

	env = "KAOHI_LISTEN_ADDR"
	config = "global.listen_address"
}

helper {
	type = "bool"
	cmdline "switch" {
		short = "h"
		long = "help"
	}

	cmdline "description" {
		short = ""
		long = "Print help message"
	}
}

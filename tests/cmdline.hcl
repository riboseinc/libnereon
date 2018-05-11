
# This HCL contains the configurations related to command line options

cmdline "log_directory" {
	type = "string"
	switch {
		short = "l"
		long = "log-dir"
	}

	description {
		short = "log directory"
		long = "Specify the path of log directory"
	}

	env = "KAOHI_LOG_DIR"
	config = "global.log_directory"
}

cmdline "log_level" {
	type = "int"
	switch {
		short = "d"
		long = "verbose"
	}

	description {
		short = "verbose level (0 ~ 3)"
		long = "Specify the verbose level"
	}

	env = "KAOHI_LOG_LEVEL"
	config = "global.log_level"
}

cmdline "listen_address" {
	type = "ipport"
	switch {
		short = "L"
		long = "listen"
	}

	description = {
		short = "IP:port"
		long = "Specify the listening address for Kaohi console"
	}

	env = "KAOHI_LISTEN_ADDR"
	config = "global.listen_address"
}

cmdline "help" {
	type = "bool"
	switch {
		short = "h"
		long = "help"
	}

	description = {
		short = ""
		long = "Print help message"
	}

	helper = true
}

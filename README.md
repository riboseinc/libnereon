
# Multi-configuration parser library using C

`libnereon` is a multi-configuration parser library implemented in C.</br>
The configuration for the library is parsed in this order:

 * The command-line arguments.
 * The environment variables.
 * The configuration files based on [HCL](https://github.com/hashicorp/hcl).

This library uses [libucl](https://github.com/vstakhov/libucl).

## HCL options for libnereon

HCL options have configuration model to specify command-line arguments, environment variables and configuration fields.</br>
Here is an example:

```

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

help {
	type = "bool"
	cmdline "switch {
		short = "h"
		long = "help"
	}

	cmdline "description" {
		short = ""
		long = "Print help message"
	}

	helper = true
}

```

 * `type`: the type of configuration value. It has `int`, `string`, `bool`, `array`, `ipport`, `float`.</br>
 * `switch`: the command-line switch.</br>
 * `description`: text message to describe each command-line option.</br>
 * `env`: the environment variable to set configuration.</br>
 * `config`: the keyword to specify an field in configuration file.</br>
 * `helper`: the flag to print help message.</br>

## libnereon API functions

All API functions are defined in [mconfig.h](https://github.com/riboseinc/libnereon/blob/master/src/mconfig.h).

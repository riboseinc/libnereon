
# LIBNEREON

[![Build Status](https://travis-ci.org/riboseinc/libnereon.svg?branch=master)](https://travis-ci.org/riboseinc/libnereon)

**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [Introduction](#introduction)
- [How to build](#how-to-build)
- [NOS and NOC syntax](#nos-and-noc-syntax)
- [libnereon API functions](#libnereon-api-functions)

## Introduction

`libnereon` is a multi-configuration parser library implemented in C.</br>
This library parses the following configurations by [HCL](https://github.com/vstakhov/libucl).

 * The command-line arguments.
 * The environment variables.
 * The configuration files.

## How To Build

To build `libnereon` on Ubuntu:

```
sudo apt install automake libtool cmake
mkdir build
cd build
cmake ..
make
```

To build `libnereon` on MacOS:

```
brew install automake libtool cmake pkg-config
mkdir build
cd build
cmake ..
make
```

## NOS and NOC syntax

`libnereon` uses NOS and NOC configurations based on HCL syntax.

### NOS configuration

`NOS` is the `NereOn configuration Schema` syntax that allows describing what configuration parameters are allowed.
It has the following HCL syntax:

```
config_option <name> {
	type = <type>

	cmdline "switch" {
		short = <short command line switch>
		long  = <long command line switch>
	}

	cmdline "description" {
		short = <short command line description>
		long  = <long command line description>
	}

	env    = <environment variable>
	config = <NOC configuration keyword>

	default = <default value>
}

```

  * `type` has the following configuration types:
     + `basic types` : `int`, `bool`, `string`, `float`, `array`
     + `config` : the configuration option with this type describes how to specify configuration file from</br>
                  command line and environment variable.
     + `helper` : the configuration option with this type describes command line options how to show help message.
  * `cmdline switch` describes short and long switch options on command line.
  * `cmdline description` describes short and long descriptions on command line.
  * `env` describes environment variable to specify the option.
  * `config` describes NOC keyword to specify the option.
  * `default` specifies default value of the option.

### NOC configuration

`NOC` is the `NereOn Configuration` syntax to allow setting configuration

# libnereon API functions

 * All API functions are defined in [nereon.h](https://github.com/riboseinc/libnereon/blob/master/src/nereon.h).
 * `NOS` configuration example is [rvd.nos](https://github.com/riboseinc/libnereon/blob/master/tests/rvd.nos).
 * `NOC` configuration example is [rvd.noc](https://github.com/riboseinc/libnereon/blob/master/tests/rvd.noc).
 * The example code is [noc_test.c](https://github.com/riboseinc/libnereon/blob/master/tests/noc_test.c).

NOTE: `NOS` configuration should be compiled with the projects which is using `libnereon`.
`nos2cc` converts `NOS` configuration to C source file.</br>
To convert `NOS` to C source file, it needs to execute the following command.</br>

```
nos2cc <NOS configuration file> <NOS CC output path> <prefix of function name>
```

The project should compile `<NOS filename>.c` and `<NOS filename>.h`, and call `get_<prefix of function name>_nos_cfg` function to get NOS contents.

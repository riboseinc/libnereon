#!/bin/bash

# check if brew is installed
readonly brew="/usr/local/bin/brew"
if [ ! -x "${brew}" ]; then
	echo "Couldn't execute '${brew}'. (Hint: install Homebrew 'https://brew.sh/')"
	exit 1
fi

# check if json-c is installed
readonly LIBUCL_DIR="$(${brew} --prefix libucl)"
if [ ! -d "${LIBUCL_DIR}" ]; then
	echo "Couldn't find libucl installation directory. (Hint: 'brew install libucl')"
	exit 1
fi

# compile project
echo "running './autogen.sh':"
./autogen.sh
./configure

echo "running 'make':"
make

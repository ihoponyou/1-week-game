# !/bin/bash

EXPECTED_ARG_COUNT=$((1))
EXPECTED_CONFIG_PATTERN="^(debug|release)$"
CONFIG_MESSAGE="(debug/release; case insensitive)"

BUILD_JOBS=$((10))

if [ $# -ne $EXPECTED_ARG_COUNT ];
then
		echo "provide a build config; $CONFIG_MESSAGE" 
		exit 1
fi

shopt -s nocasematch

if ! [[ $1 =~ $EXPECTED_CONFIG_PATTERN ]]; then
		echo "invalid build config; $CONFIG_MESSAGE"
		exit 1
fi

shopt -u nocasematch

build_config=${BASH_REMATCH[1],,}
build_config=${build_config^}

mkdir -p build
cd build
cmake ..
cmake --build ./build --config $build_config --target game -j 10 --

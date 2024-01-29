#!/bin/sh

SCRIPT_DIR=$( cd -- $( dirname -- "$( dirname -- "${BASH_SOURCE[0]}" )" ) &> /dev/null && pwd )

export LD_LIBRARY_PATH=""

export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${SCRIPT_DIR}/lib"


#!/bin/bash


SCRIPT_DIR=$( dirname -- "${BASH_SOURCE[0]}" )
PROJECT_DIR=$( cd -- $( dirname -- "${SCRIPT_DIR}" ) &> /dev/null && pwd )


export LD_LIBRARY_PATH=""
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:${PROJECT_DIR}/lib"


if [ -f ${PROJECT_DIR}/.env ]
then
  export $(cat .env | sed 's/#.*//g' | xargs)
fi

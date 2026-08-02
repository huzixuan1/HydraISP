#!/usr/bin/env bash
############################################
# This script is used to run the HydraISP testbed with a specified configuration file.
# usage: ./run.sh <config.json>
# ./run.sh ./config/isp_config_cannon.json
###########################################
set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

cd "$BUILD_DIR"

if [ $# -lt 1 ]; then
    echo "Usage: ./run.sh <config.json>"
    exit 1
fi

./isp_testbed "$1"
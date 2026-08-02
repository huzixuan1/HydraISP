#!/usr/bin/env bash
set -e

rm -rf build/*

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake ..
cmake --build . -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 4)"
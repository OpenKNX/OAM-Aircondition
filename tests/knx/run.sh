#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
BUILD_DIR=$(mktemp -d /tmp/oam-knx-tests.XXXXXX)
trap 'rm -rf "$BUILD_DIR"' EXIT
c++ -std=c++17 -g -fsanitize=address,undefined -fno-omit-frame-pointer -DKNX_NO_PRINT \
  -Isrc -Ilib/knx/src/knx tests/knx/test_energy_dpts.cpp \
  lib/knx/src/knx/dpt.cpp lib/knx/src/knx/dptconvert.cpp \
  lib/knx/src/knx/knx_value.cpp lib/knx/src/knx/bits.cpp -o "$BUILD_DIR/test_energy"
"$BUILD_DIR/test_energy"

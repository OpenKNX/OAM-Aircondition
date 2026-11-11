#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
BUILD_DIR=$(mktemp -d /tmp/oam-s21-tests.XXXXXX)
trap 'rm -rf "$BUILD_DIR"' EXIT
for TEST_MODE in default diagnostics; do
  TEST_FLAGS=""
  if [ "$TEST_MODE" = diagnostics ]; then
    TEST_FLAGS="-DDAIKIN_EXTRA_DIAGNOSTICS -DDAIKIN_PRODUCTION_DIAGNOSTICS"
  fi
  c++ -std=c++17 -g -fsanitize=address,undefined -fno-omit-frame-pointer $TEST_FLAGS \
    -Itests/daikin/stubs -Isrc/Driver/Daikin \
    tests/daikin/test_s21.cpp src/Driver/Daikin/daikin_s21_openknx_serial.cpp \
    src/Driver/Daikin/DaikinDriver.cpp src/AirConditionDriver.cpp -o "$BUILD_DIR/test_s21"
  "$BUILD_DIR/test_s21"
done

#!/usr/bin/env sh

./scripts/run-clang-format.py --clang-format-executable=${CLANG_FORMAT_PATH} -r server/ client/ shared/ tests/

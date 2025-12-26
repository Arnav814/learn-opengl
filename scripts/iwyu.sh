#! /usr/bin/env bash
set -euo pipefail

processing="$(realpath "$1")"
clang_cmd="$(jq -r ".[] | select(.file == \"$processing\").command" ./build/compile_commands.json)"
clang_args="$(grep -oP '(?<= ).*' <<< "$clang_cmd" | sed -e 's/\\"/"/g')"

iwyu -Xiwyu --keep="common.hpp" -Xiwyu --cxx17ns $clang_args "$@"


#! /usr/bin/env bash
set -euo pipefail

# only run iwyu on files that are actually from this project
source_dir="$(realpath "$(dirname "$(realpath "$0")")"/../src)"
filtered_compile_commands="$(mktemp --suffix=.json)"
jq ".[] | select(.file | startswith(\"$source_dir\"))" ./build/compile_commands.json \
	| jq --slurp > "$filtered_compile_commands"

iwyu_tool "-j$(nproc)" -p "$filtered_compile_commands" "$@"

rm "$filtered_compile_commands"


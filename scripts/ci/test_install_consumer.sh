#!/usr/bin/env bash
set -euo pipefail

repo="${1:-$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)}"
build="${2:-${repo}/build}"
work="${3:-${build}/install-consumer-validation}"

if [[ -d "${repo}/cpp/tests/install_consumer" ]]; then
    consumer_source="${repo}/cpp/tests/install_consumer"
else
    consumer_source="${repo}/tests/install_consumer"
fi

rm -rf "${work}"
cmake -S "${repo}" -B "${build}" -GNinja -DCMAKE_BUILD_TYPE=Debug
cmake --build "${build}" -- -j"${ELIZAOS_BUILD_JOBS:-2}"
cmake --install "${build}" --prefix "${work}/prefix"
cmake -S "${consumer_source}" -B "${work}/build" -GNinja \
    -DCMAKE_PREFIX_PATH="${work}/prefix"
cmake --build "${work}/build" -- -j"${ELIZAOS_BUILD_JOBS:-2}"
"${work}/build/elizaos_installed_consumer"

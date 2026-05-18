#!/usr/bin/env bash
set -euo pipefail

root_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${BUILD_DIR:-$root_dir/build-autonomy-e2e}"
cmake_build_type="${CMAKE_BUILD_TYPE:-Release}"
ctest_timeout="${CTEST_TIMEOUT:-30}"

cmake -S "$root_dir" -B "$build_dir" \
  -DCMAKE_BUILD_TYPE="$cmake_build_type" \
  -DBUILD_TESTS=ON \
  -DBUILD_BENCHMARKS=OFF

# Build only targets that exist in this checkout; this prevents false CTest
# "Not Run" failures while still exercising every available autonomy package.
mapfile -t available_targets < <(cmake --build "$build_dir" --target help | awk '/^\.\.\. / {print $2}' | sort -u)
has_target() {
  local needle="$1"
  printf '%s\n' "${available_targets[@]}" | grep -qx "$needle"
}

targets=(
  agentbrowser_test
  agentcomms_test
  real_agentaction_test
  real_agentagenda_test
  real_agentbrowser_test
  real_agentcomms_test
  real_agentloop_test
  real_agentmemory_test
)

selected_tests=()
for target in "${targets[@]}"; do
  if has_target "$target"; then
    cmake --build "$build_dir" --target "$target" --parallel "${BUILD_JOBS:-2}"
    selected_tests+=("$target")
  fi
done

if ((${#selected_tests[@]} == 0)); then
  echo "No autonomy E2E targets were found in $build_dir" >&2
  exit 1
fi

regex="^($(IFS='|'; echo "${selected_tests[*]}"))$"
ctest --test-dir "$build_dir" --output-on-failure --timeout "$ctest_timeout" -R "$regex"

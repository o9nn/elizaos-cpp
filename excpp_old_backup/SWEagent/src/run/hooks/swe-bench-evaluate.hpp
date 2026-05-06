#include "..types.hpp"
#include "..utils/log.hpp"
#include ".merge-predictions.hpp"
#include "types.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * SweBench evaluation hook.
 * Will be automatically added to `run_batch` if `SWEBenchInstances.evaluate` is set to true
 */

/**
 * SweBench evaluation hook
 */
class SweBenchEvaluate extends AbstractRunHook {
  private static readonly SUBSET_MAP: Record<string, string> = {
    lite: 'swe-bench_lite',
    verified: 'swe-bench_verified',
    multimodal: 'swe-bench_multimodal',
  };

    // We need to add a suffix to the run_id to avoid collisions when you reuse the name of your run

  /**
   * Warn if one of the running calls failed
   */

    // Use a simple lock mechanism

  /**
   * Move report from `sb-cli-reports` to `results.json`
   */

      // Remove temporary predictions if they exist


} // namespace elizaos

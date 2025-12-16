#include "..agent/problem-statement.hpp"
#include "..environment/repo.hpp"
#include "..environment/swe-env.hpp"
#include "..types.hpp"
#include "..utils/log.hpp"
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
 * Hook for saving and applying patches
 */

/**
 * Check if patch is promising (likely to solve the issue)
 */
bool isPromisingPatch(AgentInfo info);

/**
 * Print patch message to console
 */
void printPatchMessage(const std::string& patchOutputFile);

/**
 * This hook saves patches to a separate directory and optionally applies them to a local repository
 */
class SaveApplyPatchHook extends AbstractRunHook {
  private applyPatchLocally: boolean;
  private showSuccessMessage: boolean;
  private outputDir?: string;
  private env?: SWEEnv;
  private problemStatement?: ProblemStatementConfig;

  constructor(applyPatchLocally: boolean = false, showSuccessMessage: boolean = true) {
    super();
    this.applyPatchLocally = applyPatchLocally;
    this.showSuccessMessage = showSuccessMessage;
  }

  /**
   * Create patch files that can be applied with `git am`.
   * Returns the path to the patch file if it was saved, otherwise null.
   */

    // Create directory if it doesn't exist

      // Only print big congratulations if we actually believe
      // the patch will solve the issue

  /**
   * Apply a patch to a local directory
   */

    // The resolve() is important, because we're gonna run the cmd
    // somewhere else


} // namespace elizaos

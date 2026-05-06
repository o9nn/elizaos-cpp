#include "..agent/problem-statement.hpp"
#include "..environment/swe-env.hpp"
#include "..types.hpp"
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
 * Type definitions for run hooks
 */

/**
 * Hook structure for the web server or other addons to interface with
 */
struct RunHook {
    double index;
    SWEEnv env;
    ProblemStatement | ProblemStatementConfig problemStatement;
};

/**
 * Abstract base class for run hooks
 */
    // Default implementation - can be overridden

    // Default implementation - can be overridden

    // Default implementation - can be overridden

    // Default implementation - can be overridden

    // Default implementation - can be overridden

    // Default implementation - can be overridden

/**
 * Combined run hooks manager
 */
class CombinedRunHooks implements RunHook {
  private _hooks: RunHook[] = [];

  addHook(hook: RunHook): void {
    this._hooks.push(hook);
  }


} // namespace elizaos

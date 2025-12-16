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

;
;
;

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
abstract class AbstractRunHook implements RunHook {
  onInit(_run: Record<string, unknown>): void {
    // Default implementation - can be overridden
  }

  onStart(): void {
    // Default implementation - can be overridden
  }

  onEnd(): void {
    // Default implementation - can be overridden
  }

  onInstanceStart(_params: {
    index: number;
    env: SWEEnv;
    problemStatement: ProblemStatement | ProblemStatementConfig;
  }): void {
    // Default implementation - can be overridden
  }

  onInstanceSkipped(): void {
    // Default implementation - can be overridden
  }

  onInstanceCompleted(_params: { result: AgentRunResult }): void {
    // Default implementation - can be overridden
  }
}

/**
 * Combined run hooks manager
 */
class CombinedRunHooks implements RunHook {
  private _hooks: RunHook[] = [];

  addHook(hook: RunHook): void {
    this._hooks.push(hook);
  }

  get hooks(): RunHook[] {
    return this._hooks;
  }

  onInit(run: Record<string, unknown>): void {
    for (const hook of this._hooks) {
      hook.onInit(run);
    }
  }

  onStart(): void {
    for (const hook of this._hooks) {
      hook.onStart();
    }
  }

  onEnd(): void {
    for (const hook of this._hooks) {
      hook.onEnd();
    }
  }

  onInstanceStart(params: {
    index: number;
    env: SWEEnv;
    problemStatement: ProblemStatement | ProblemStatementConfig;
  }): void {
    for (const hook of this._hooks) {
      hook.onInstanceStart(params);
    }
  }

  onInstanceSkipped(): void {
    for (const hook of this._hooks) {
      hook.onInstanceSkipped();
    }
  }

  onInstanceCompleted(params: { result: AgentRunResult }): void {
    for (const hook of this._hooks) {
      hook.onInstanceCompleted(params);
    }
  }
}

} // namespace elizaos

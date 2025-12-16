#include ".repo.hpp"
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
 * Environment hooks
 * Converted from sweagent/environment/hooks/abstract.py
 */

;

// Environment interface for hooks
struct EnvironmentInstance {
    std::optional<unknown> deployment;
    std::optional<Repo | null> repo;
};


/**
 * Abstract environment hook interface
 */
abstract class EnvHook {
  /**
   * Called when the environment is initialized
   */
  onInit(_env: EnvironmentInstance): void {
    // Default implementation - do nothing
  }

  /**
   * Called when repository copying starts
   */
  onCopyRepoStarted(_repo: Repo | RepoConfig): void {
    // Default implementation - do nothing
  }

  /**
   * Called when deployment starts
   */
  onStartDeployment(): void {
    // Default implementation - do nothing
  }

  /**
   * Called when environment installation starts
   */
  onInstallEnvStarted(): void {
    // Default implementation - do nothing
  }

  /**
   * Called when the environment is closing
   */
  onClose(): void {
    // Default implementation - do nothing
  }

  /**
   * Called when the environment startup is complete
   */
  onEnvironmentStartup(): void {
    // Default implementation - do nothing
  }
}

/**
 * Combined environment hooks - allows multiple hooks to be registered
 */
class CombinedEnvHooks extends EnvHook {
  private hooks: EnvHook[] = [];

  addHook(hook: EnvHook): void {
    this.hooks.push(hook);
  }

  onInit(env: EnvironmentInstance): void {
    for (const hook of this.hooks) {
      try {
        hook.onInit(env);
      } catch (error) {
        console.error('Hook error in onInit:', error);
        // Continue with other hooks
      }
    }
  }

  onCopyRepoStarted(repo: Repo | RepoConfig): void {
    for (const hook of this.hooks) {
      try {
        hook.onCopyRepoStarted(repo);
      } catch (error) {
        console.error('Hook error in onCopyRepoStarted:', error);
        // Continue with other hooks
      }
    }
  }

  onStartDeployment(): void {
    for (const hook of this.hooks) {
      try {
        hook.onStartDeployment();
      } catch (error) {
        console.error('Hook error in onStartDeployment:', error);
        // Continue with other hooks
      }
    }
  }

  onInstallEnvStarted(): void {
    for (const hook of this.hooks) {
      try {
        hook.onInstallEnvStarted();
      } catch (error) {
        console.error('Hook error in onInstallEnvStarted:', error);
        // Continue with other hooks
      }
    }
  }

  onClose(): void {
    for (const hook of this.hooks) {
      try {
        hook.onClose();
      } catch (error) {
        console.error('Hook error in onClose:', error);
        // Continue with other hooks
      }
    }
  }

  onEnvironmentStartup(): void {
    for (const hook of this.hooks) {
      try {
        hook.onEnvironmentStartup();
      } catch (error) {
        console.error('Hook error in onEnvironmentStartup:', error);
        // Continue with other hooks
      }
    }
  }
}

} // namespace elizaos

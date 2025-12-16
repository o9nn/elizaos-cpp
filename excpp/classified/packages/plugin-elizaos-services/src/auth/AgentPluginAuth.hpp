#include "AuthenticationService.js.hpp"
#include "CLIAuthCommands.js.hpp"
#include "elizaos/core.hpp"
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
 * Agent Plugin Authentication Integration
 * Integrates authentication service with ElizaOS plugin runtime
 */

/**
 * Authentication Service for Plugin Integration
 */
class AgentAuthService extends Service {
  capabilityDescription = 'Manages API key authentication and validation for ElizaOS services';

  private authService: AuthenticationService;

  constructor(runtime?: IAgentRuntime) {
    super(runtime);
    if (!runtime) {
      throw new Error('Runtime is required for AgentAuthService');
    }
    this.authService = new AuthenticationService(runtime);
  }

    // Perform initial auth check

  /**
   * Get current authentication status
   */

  /**
   * Validate specific API key
   */

  /**
   * Test API functionality
   */

  /**
   * Check if provider is authenticated and capable
   */

  /**
   * Get the best available provider for a capability
   */

      // Find all providers that support the capability

      // Prefer production keys over test keys

        // Return first production provider (could add load balancing logic here)

      // Fall back to test providers

  /**
   * Validate all providers and return summary
   */

  /**
   * Clear authentication cache
   */

  /**
   * Stop the service and cleanup resources
   */

/**
 * Plugin Integration for Authentication
 */

    // Register CLI commands if in CLI context

    // Validate configuration on startup

/**
 * Helper functions for plugin usage
 */
class AuthHelper {
  /**
   * Check if a provider is ready for use
   */
  static async isProviderReady(
    runtime: IAgentRuntime,
    provider: string,
    capability?: string
  ): Promise<boolean> {
    const authService = runtime.getService<AgentAuthService>('elizaos-services-auth');
    if (!authService) {
      logger.warn('Authentication service not available');
      return false;
    }

    return authService.isProviderReady(provider, capability);
  }

  /**
   * Get the best provider for a capability
   */

  /**
   * Validate provider before using in model calls
   */

  /**
   * Get authentication status for debugging
   */


} // namespace elizaos

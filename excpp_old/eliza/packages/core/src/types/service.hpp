#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Core service type registry that can be extended by plugins via module augmentation.
 * Plugins can extend this interface to add their own service types:
 *
 * @example
 * ```typescript
 * declare module '@elizaos/core' {
 *   struct ServiceTypeRegistry {

 * }
 * ```
 */
struct ServiceTypeRegistry {
    'transcription' TRANSCRIPTION;
    'video' VIDEO;
    'browser' BROWSER;
    'pdf' PDF;
    'aws_s3' REMOTE_FILES;
    'web_search' WEB_SEARCH;
    'email' EMAIL;
    'tee' TEE;
    'task' TASK;
    'wallet' WALLET;
    'lp_pool' LP_POOL;
    'token_data' TOKEN_DATA;
    'database_migration' DATABASE_MIGRATION;
    'PLUGIN_MANAGER' PLUGIN_MANAGER;
    'PLUGIN_CONFIGURATION' PLUGIN_CONFIGURATION;
    'PLUGIN_USER_INTERACTION' PLUGIN_USER_INTERACTION;
};

/**
 * Type for service names that includes both core services and any plugin-registered services
 */
using ServiceTypeName = ServiceTypeRegistry[keyof ServiceTypeRegistry];

/**
 * Helper type to extract service type values from the registry
 */

/**
 * Helper type to check if a service type exists in the registry
 */

/**
 * Type-safe service class definition
 */

/**
 * Map of service type names to their implementation classes
 */
struct ServiceClassMap {
};

/**
 * Helper to infer service instance type from service type name
 */

/**
 * Runtime service registry type
 */

/**
 * Enumerates the recognized types of services that can be registered and used by the agent runtime.
 * Services provide specialized functionalities like audio transcription, video processing,
 * web browsing, PDF handling, file storage (e.g., AWS S3), web search, email integration,
 * secure execution via TEE (Trusted Execution Environment), and task management.
 * This constant is used in `AgentRuntime` for service registration and retrieval (e.g., `getService`).
 * Each service typically implements the `Service` abstract class or a more specific interface like `IVideoService`.
 */

/**
 * Client instance
 */
    std::future<void> stop();
    std::future<unknown> stop(IAgentRuntime _runtime);

/**
 * Generic service interface that provides better type checking for services
 * @template ConfigType The configuration type for this service
 * @template ResultType The result type returned by the service operations
 */
  /**
   * The configuration for this service instance
   */

  /**
   * Process an input with this service
   * @param input The input to process
   * @returns A promise resolving to the result
   */

/**
 * Generic factory function to create a typed service instance
 * @param runtime The agent runtime
 * @param serviceType The type of service to get
 * @returns The service instance or null if not available
 */

/**
 * Standardized service error type for consistent error handling
 */
struct ServiceError {
    std::string code;
    std::string message;
    std::optional<unknown> details;
    std::optional<Error> cause;
};

/**
 * Safely create a ServiceError from any caught error
 */
ServiceError createServiceError(unknown error, auto code = 'UNKNOWN_ERROR');

} // namespace elizaos

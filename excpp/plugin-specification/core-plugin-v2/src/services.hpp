#include "elizaos/core.hpp"
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

;

;

/**
 * Service builder class that provides type-safe service creation
 * with automatic type inference
 */
class ServiceBuilder<TService extends Service = Service> extends coreServiceBuilder {
  constructor(serviceType: ServiceTypeName | string) {
    super(serviceType)
    //this.serviceType = serviceType;
    this.description = '';
  }
}

/**
 * Create a type-safe service builder
 * @param serviceType - The service type name
 * @returns A new ServiceBuilder instance
 */


/**
 * Type-safe service definition helper
 */
interface ServiceDefinition<T extends Service = Service> {
  serviceType: ServiceTypeName;
  description: string;
  start: (runtime: IAgentRuntime) => Promise<T>;
  stop?: () => Promise<void>;
}

/**
 * Define a service with type safety
 */


} // namespace elizaos

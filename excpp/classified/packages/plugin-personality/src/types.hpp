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

// Extend the core service types with character management service
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'CHARACTER_MANAGEMENT' CHARACTER_MANAGEMENT;
};

}

// Export service type constant
const PersonalityServiceType = {
  CHARACTER_MANAGEMENT: 'CHARACTER_MANAGEMENT' as const,
} satisfies Partial<import('@elizaos/core').ServiceTypeRegistry>;

} // namespace elizaos

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

// Extend the core service types with stagehand service
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'STAGEHAND' STAGEHAND;
};

}

// Export service type constant
const StagehandServiceType = {
  STAGEHAND: 'STAGEHAND' as const,
} satisfies Partial<import('@elizaos/core').ServiceTypeRegistry>;

} // namespace elizaos

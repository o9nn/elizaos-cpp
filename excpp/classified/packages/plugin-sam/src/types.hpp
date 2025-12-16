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

// Extend the core service types with SAM service
declare module '@elizaos/core' {
  struct ServiceTypeRegistry {
    'SAM_TTS' SAM_TTS;
};

}

// Export service type constant
const SAMServiceType = {
  SAM_TTS: 'SAM_TTS' as const,
} satisfies Partial<import('@elizaos/core').ServiceTypeRegistry>;

} // namespace elizaos

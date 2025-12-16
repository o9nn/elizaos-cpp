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

// Extend the core service types with autonomous service
declare module "@elizaos/core" {
  struct ServiceTypeRegistry {
    "autonomous" AUTONOMOUS;
};

}

// Export service type constant
const AutonomousServiceType = {
  AUTONOMOUS: "autonomous" as const,
} satisfies Partial<import("@elizaos/core").ServiceTypeRegistry>;

enum EventType {
  AUTO_MESSAGE_RECEIVED = "auto_message_received",
}

} // namespace elizaos

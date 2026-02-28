#include "agent-memory-edit-overlay.hpp"

namespace elizaos {
namespace eliza_client {

bool AgentMemoryEditOverlay::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentMemoryEditOverlay::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentMemoryEditOverlay::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

#include "agent-sidebar.hpp"

namespace elizaos {
namespace eliza_client {

bool AgentSidebar::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentSidebar::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentSidebar::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

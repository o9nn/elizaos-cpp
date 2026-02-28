#include "agent-log-viewer.hpp"

namespace elizaos {
namespace eliza_client {

bool AgentLogViewer::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentLogViewer::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentLogViewer::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

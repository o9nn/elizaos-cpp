#include "agent-templates.hpp"

namespace elizaos {
namespace eliza_client {

bool AgentTemplates::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentTemplates::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentTemplates::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

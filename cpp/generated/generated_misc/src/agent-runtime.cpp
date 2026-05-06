#include "agent-runtime.hpp"

namespace elizaos {
namespace generated_misc {

bool AgentRuntime::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentRuntime::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentRuntime::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

#include "agent.hpp"

namespace elizaos {
namespace eliza_core {

bool Agent::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Agent::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Agent::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

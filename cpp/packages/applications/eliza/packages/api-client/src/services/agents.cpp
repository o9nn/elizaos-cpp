#include "agents.hpp"

namespace elizaos {
namespace eliza_api_client {

bool Agents::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Agents::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Agents::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_api_client
} // namespace elizaos

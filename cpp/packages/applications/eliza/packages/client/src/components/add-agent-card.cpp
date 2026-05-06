#include "add-agent-card.hpp"

namespace elizaos {
namespace eliza_client {

bool AddAgentCard::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AddAgentCard::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AddAgentCard::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

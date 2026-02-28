#include "agent-chat.cy.hpp"

namespace elizaos {
namespace generated_testing {

bool AgentChatCy::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentChatCy::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentChatCy::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

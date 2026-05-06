#include "agent-integration.test.hpp"

namespace elizaos {
namespace generated_testing {

bool AgentIntegrationTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AgentIntegrationTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AgentIntegrationTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

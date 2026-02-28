#include "claude-code-integration.test.hpp"

namespace elizaos {
namespace generated_testing {

bool ClaudeCodeIntegrationTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ClaudeCodeIntegrationTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ClaudeCodeIntegrationTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

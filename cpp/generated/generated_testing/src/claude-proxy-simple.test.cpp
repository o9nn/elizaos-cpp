#include "claude-proxy-simple.test.hpp"

namespace elizaos {
namespace generated_testing {

bool ClaudeProxySimpleTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ClaudeProxySimpleTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ClaudeProxySimpleTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

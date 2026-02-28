#include "elizaos-runtime.test.hpp"

namespace elizaos {
namespace generated_testing {

bool ElizaosRuntimeTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ElizaosRuntimeTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ElizaosRuntimeTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

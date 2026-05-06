#include "bun-exec.test.hpp"

namespace elizaos {
namespace generated_testing {

bool BunExecTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void BunExecTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json BunExecTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

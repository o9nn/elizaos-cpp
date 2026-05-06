#include "timeout-manager.test.hpp"

namespace elizaos {
namespace generated_testing {

bool TimeoutManagerTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TimeoutManagerTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TimeoutManagerTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

#include "test-quick-stats.test.hpp"

namespace elizaos {
namespace generated_testing {

bool TestQuickStatsTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TestQuickStatsTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TestQuickStatsTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

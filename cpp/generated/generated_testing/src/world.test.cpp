#include "world.test.hpp"

namespace elizaos {
namespace generated_testing {

bool WorldTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void WorldTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json WorldTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

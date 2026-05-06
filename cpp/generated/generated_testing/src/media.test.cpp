#include "media.test.hpp"

namespace elizaos {
namespace generated_testing {

bool MediaTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void MediaTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json MediaTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

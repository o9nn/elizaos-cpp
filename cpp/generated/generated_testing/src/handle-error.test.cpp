#include "handle-error.test.hpp"

namespace elizaos {
namespace generated_testing {

bool HandleErrorTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void HandleErrorTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json HandleErrorTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

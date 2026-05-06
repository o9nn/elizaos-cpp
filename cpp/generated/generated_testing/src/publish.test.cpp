#include "publish.test.hpp"

namespace elizaos {
namespace generated_testing {

bool PublishTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PublishTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PublishTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

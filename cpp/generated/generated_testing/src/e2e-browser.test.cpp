#include "e2e-browser.test.hpp"

namespace elizaos {
namespace generated_testing {

bool E2eBrowserTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void E2eBrowserTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json E2eBrowserTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

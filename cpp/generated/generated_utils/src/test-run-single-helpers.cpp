#include "test-run-single-helpers.hpp"

namespace elizaos {
namespace generated_utils {

bool TestRunSingleHelpers::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TestRunSingleHelpers::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TestRunSingleHelpers::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_utils
} // namespace elizaos

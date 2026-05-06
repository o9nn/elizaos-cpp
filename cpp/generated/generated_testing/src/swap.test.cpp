#include "swap.test.hpp"

namespace elizaos {
namespace generated_testing {

bool SwapTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void SwapTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json SwapTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

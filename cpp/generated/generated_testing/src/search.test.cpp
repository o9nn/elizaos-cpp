#include "search.test.hpp"

namespace elizaos {
namespace generated_testing {

bool SearchTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void SearchTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json SearchTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

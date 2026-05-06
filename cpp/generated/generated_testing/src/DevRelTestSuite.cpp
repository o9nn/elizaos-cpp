#include "DevRelTestSuite.hpp"

namespace elizaos {
namespace generated_testing {

bool Devreltestsuite::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Devreltestsuite::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Devreltestsuite::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

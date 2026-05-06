#include "ambiguity-handling.test.hpp"

namespace elizaos {
namespace generated_testing {

bool AmbiguityHandlingTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AmbiguityHandlingTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AmbiguityHandlingTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

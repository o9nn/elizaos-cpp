#include "liaison.test.hpp"

namespace elizaos {
namespace generated_testing {

bool LiaisonTest::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void LiaisonTest::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json LiaisonTest::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

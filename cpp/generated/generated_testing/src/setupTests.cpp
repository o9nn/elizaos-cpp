#include "setupTests.hpp"

namespace elizaos {
namespace generated_testing {

bool Setuptests::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Setuptests::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Setuptests::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_testing
} // namespace elizaos

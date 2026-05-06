#include "LoginScreen.hpp"

namespace elizaos {
namespace mobile {

bool Loginscreen::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Loginscreen::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Loginscreen::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

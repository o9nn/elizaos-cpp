#include "PasskeyLogin.hpp"

namespace elizaos {
namespace mobile {

bool Passkeylogin::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Passkeylogin::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Passkeylogin::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

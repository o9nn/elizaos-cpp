#include "privacy-policy.hpp"

namespace elizaos {
namespace autofun_client {

bool PrivacyPolicy::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PrivacyPolicy::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PrivacyPolicy::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

#include "SMS.hpp"

namespace elizaos {
namespace mobile {

bool Sms::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Sms::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Sms::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

#include "messaging.hpp"

namespace elizaos {
namespace eliza_api_client {

bool Messaging::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Messaging::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Messaging::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_api_client
} // namespace elizaos

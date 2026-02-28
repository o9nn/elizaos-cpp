#include "base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

bool BaseClient::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void BaseClient::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json BaseClient::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_api_client
} // namespace elizaos

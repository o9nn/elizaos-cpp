#include "websocket-client.hpp"

namespace elizaos {
namespace generated_cli {

bool WebsocketClient::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void WebsocketClient::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json WebsocketClient::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_cli
} // namespace elizaos

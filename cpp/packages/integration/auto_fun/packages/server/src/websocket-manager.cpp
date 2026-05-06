#include "websocket-manager.hpp"

namespace elizaos {
namespace autofun_server {

bool WebsocketManager::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void WebsocketManager::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json WebsocketManager::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

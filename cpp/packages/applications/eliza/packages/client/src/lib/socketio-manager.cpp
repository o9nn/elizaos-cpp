#include "socketio-manager.hpp"

namespace elizaos {
namespace eliza_client {

bool SocketioManager::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void SocketioManager::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json SocketioManager::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

#include "socket.hpp"

namespace elizaos {
namespace autofun_client {

bool Socket::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Socket::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Socket::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

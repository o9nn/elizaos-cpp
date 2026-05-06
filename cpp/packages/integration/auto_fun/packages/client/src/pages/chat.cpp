#include "chat.hpp"

namespace elizaos {
namespace autofun_client {

bool Chat::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Chat::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Chat::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

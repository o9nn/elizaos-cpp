#include "connection-status.hpp"

namespace elizaos {
namespace eliza_client {

bool ConnectionStatus::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ConnectionStatus::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ConnectionStatus::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

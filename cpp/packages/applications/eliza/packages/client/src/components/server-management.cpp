#include "server-management.hpp"

namespace elizaos {
namespace eliza_client {

bool ServerManagement::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ServerManagement::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ServerManagement::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

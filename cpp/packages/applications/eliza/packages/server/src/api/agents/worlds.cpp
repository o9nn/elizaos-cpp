#include "worlds.hpp"

namespace elizaos {
namespace eliza_server {

bool Worlds::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Worlds::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Worlds::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_server
} // namespace elizaos

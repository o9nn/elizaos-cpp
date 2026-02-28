#include "raydium.hpp"

namespace elizaos {
namespace autofun_server {

bool Raydium::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Raydium::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Raydium::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

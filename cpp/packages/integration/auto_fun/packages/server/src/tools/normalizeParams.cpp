#include "normalizeParams.hpp"

namespace elizaos {
namespace autofun_server {

bool Normalizeparams::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Normalizeparams::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Normalizeparams::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

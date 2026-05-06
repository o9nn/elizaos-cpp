#include "pdas.hpp"

namespace elizaos {
namespace autofun_raydium {

bool Pdas::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Pdas::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Pdas::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_raydium
} // namespace elizaos

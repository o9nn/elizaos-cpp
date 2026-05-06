#include "raydium_vault.hpp"

namespace elizaos {
namespace autofun_types {

bool RaydiumVault::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void RaydiumVault::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json RaydiumVault::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_types
} // namespace elizaos

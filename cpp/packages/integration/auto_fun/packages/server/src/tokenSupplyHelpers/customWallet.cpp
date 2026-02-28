#include "customWallet.hpp"

namespace elizaos {
namespace autofun_server {

bool Customwallet::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Customwallet::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Customwallet::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

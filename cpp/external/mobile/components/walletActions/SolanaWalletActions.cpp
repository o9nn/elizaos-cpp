#include "SolanaWalletActions.hpp"

namespace elizaos {
namespace mobile {

bool Solanawalletactions::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Solanawalletactions::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Solanawalletactions::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

#include "sol-price-provider.hpp"

namespace elizaos {
namespace autofun_client {

bool SolPriceProvider::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void SolPriceProvider::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json SolPriceProvider::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

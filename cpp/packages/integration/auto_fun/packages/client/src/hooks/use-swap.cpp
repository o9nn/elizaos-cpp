#include "use-swap.hpp"

namespace elizaos {
namespace autofun_client {

bool UseSwap::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UseSwap::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UseSwap::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

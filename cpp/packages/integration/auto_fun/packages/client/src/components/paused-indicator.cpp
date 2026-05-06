#include "paused-indicator.hpp"

namespace elizaos {
namespace autofun_client {

bool PausedIndicator::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PausedIndicator::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PausedIndicator::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

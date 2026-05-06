#include "use-partial-update.hpp"

namespace elizaos {
namespace eliza_client {

bool UsePartialUpdate::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UsePartialUpdate::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UsePartialUpdate::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

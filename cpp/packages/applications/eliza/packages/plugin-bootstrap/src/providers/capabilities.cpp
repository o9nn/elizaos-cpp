#include "capabilities.hpp"

namespace elizaos {
namespace eliza_plugin_bootstrap {

bool Capabilities::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Capabilities::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Capabilities::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

#include "pluginStateProvider.hpp"

namespace elizaos {
namespace generated_plugins {

bool Pluginstateprovider::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Pluginstateprovider::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Pluginstateprovider::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_plugins
} // namespace elizaos

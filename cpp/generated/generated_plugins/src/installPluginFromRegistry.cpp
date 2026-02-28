#include "installPluginFromRegistry.hpp"

namespace elizaos {
namespace generated_plugins {

bool Installpluginfromregistry::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Installpluginfromregistry::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Installpluginfromregistry::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_plugins
} // namespace elizaos

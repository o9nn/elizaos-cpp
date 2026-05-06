#include "unloadPlugin.hpp"

namespace elizaos {
namespace generated_plugins {

bool Unloadplugin::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Unloadplugin::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Unloadplugin::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_plugins
} // namespace elizaos

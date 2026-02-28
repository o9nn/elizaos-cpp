#include "loadPlugin.hpp"

namespace elizaos {
namespace generated_plugins {

bool Loadplugin::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Loadplugin::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Loadplugin::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_plugins
} // namespace elizaos

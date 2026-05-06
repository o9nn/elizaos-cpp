#include "starter-plugin.hpp"

namespace elizaos {
namespace eliza_plugin_starter {

bool StarterPlugin::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void StarterPlugin::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json StarterPlugin::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_starter
} // namespace elizaos

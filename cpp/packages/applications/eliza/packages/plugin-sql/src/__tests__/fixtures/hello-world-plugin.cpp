#include "hello-world-plugin.hpp"

namespace elizaos {
namespace eliza_plugin_sql {

bool HelloWorldPlugin::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void HelloWorldPlugin::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json HelloWorldPlugin::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_sql
} // namespace elizaos

#include "reflection.hpp"

namespace elizaos {
namespace eliza_plugin_bootstrap {

bool Reflection::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Reflection::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Reflection::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

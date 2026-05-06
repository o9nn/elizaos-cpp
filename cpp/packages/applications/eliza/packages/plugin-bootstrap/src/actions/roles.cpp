#include "roles.hpp"

namespace elizaos {
namespace eliza_plugin_bootstrap {

bool Roles::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Roles::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Roles::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

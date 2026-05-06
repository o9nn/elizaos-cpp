#include "updateEntity.hpp"

namespace elizaos {
namespace eliza_plugin_bootstrap {

bool Updateentity::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Updateentity::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Updateentity::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

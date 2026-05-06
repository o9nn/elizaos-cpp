#include "plugin.hpp"

namespace elizaos {
namespace eliza_project_tee_starter {

bool Plugin::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Plugin::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Plugin::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_project_tee_starter
} // namespace elizaos

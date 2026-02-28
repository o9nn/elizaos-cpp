#include "vite-env.hpp"

namespace elizaos {
namespace eliza_app {

bool ViteEnv::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ViteEnv::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ViteEnv::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_app
} // namespace elizaos

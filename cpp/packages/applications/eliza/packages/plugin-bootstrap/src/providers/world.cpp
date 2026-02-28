#include "world.hpp"

namespace elizaos {
namespace eliza_plugin_bootstrap {

bool World::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void World::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json World::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

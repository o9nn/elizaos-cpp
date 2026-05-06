#include "entity-seed.hpp"

namespace elizaos {
namespace eliza_plugin_sql {

bool EntitySeed::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void EntitySeed::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json EntitySeed::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_sql
} // namespace elizaos

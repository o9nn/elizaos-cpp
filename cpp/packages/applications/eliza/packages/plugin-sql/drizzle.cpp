#include "drizzle.hpp"

namespace elizaos {
namespace eliza_plugin_sql {

bool Drizzle::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Drizzle::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Drizzle::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_sql
} // namespace elizaos

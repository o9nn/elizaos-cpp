#include "cache.hpp"

namespace elizaos {
namespace eliza_plugin_sql {

bool Cache::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Cache::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Cache::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_sql
} // namespace elizaos

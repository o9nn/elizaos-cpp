#include "factory.hpp"

namespace elizaos {
namespace eliza_plugin_sql {

bool Factory::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Factory::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Factory::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_sql
} // namespace elizaos

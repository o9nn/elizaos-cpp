#include "time.hpp"

namespace elizaos {
namespace eliza_plugin_bootstrap {

bool Time::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Time::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Time::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_plugin_bootstrap
} // namespace elizaos

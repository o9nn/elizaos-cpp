#include "events.hpp"

namespace elizaos {
namespace eliza_core {

bool Events::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Events::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Events::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

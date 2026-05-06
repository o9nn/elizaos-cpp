#include "registry.hpp"

namespace elizaos {
namespace generated_misc {

bool Registry::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Registry::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Registry::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

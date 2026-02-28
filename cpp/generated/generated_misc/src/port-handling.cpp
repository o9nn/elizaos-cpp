#include "port-handling.hpp"

namespace elizaos {
namespace generated_misc {

bool PortHandling::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void PortHandling::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json PortHandling::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

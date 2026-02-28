#include "demo-interactive.hpp"

namespace elizaos {
namespace generated_misc {

bool DemoInteractive::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void DemoInteractive::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json DemoInteractive::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

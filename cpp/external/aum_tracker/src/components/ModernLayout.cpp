#include "ModernLayout.hpp"

namespace elizaos {
namespace aum_tracker {

bool Modernlayout::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Modernlayout::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Modernlayout::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace aum_tracker
} // namespace elizaos

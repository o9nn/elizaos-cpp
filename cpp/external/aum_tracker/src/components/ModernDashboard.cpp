#include "ModernDashboard.hpp"

namespace elizaos {
namespace aum_tracker {

bool Moderndashboard::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Moderndashboard::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Moderndashboard::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace aum_tracker
} // namespace elizaos

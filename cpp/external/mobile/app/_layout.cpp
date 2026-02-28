#include "_layout.hpp"

namespace elizaos {
namespace mobile {

bool Layout::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Layout::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Layout::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

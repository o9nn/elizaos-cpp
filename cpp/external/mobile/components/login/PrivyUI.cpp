#include "PrivyUI.hpp"

namespace elizaos {
namespace mobile {

bool Privyui::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Privyui::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Privyui::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

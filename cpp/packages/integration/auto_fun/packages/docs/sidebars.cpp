#include "sidebars.hpp"

namespace elizaos {
namespace autofun_docs {

bool Sidebars::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Sidebars::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Sidebars::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_docs
} // namespace elizaos

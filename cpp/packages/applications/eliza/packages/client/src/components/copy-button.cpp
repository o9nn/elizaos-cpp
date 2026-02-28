#include "copy-button.hpp"

namespace elizaos {
namespace eliza_client {

bool CopyButton::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void CopyButton::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json CopyButton::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

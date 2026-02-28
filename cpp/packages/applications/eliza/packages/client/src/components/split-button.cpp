#include "split-button.hpp"

namespace elizaos {
namespace eliza_client {

bool SplitButton::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void SplitButton::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json SplitButton::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

#include "playwright.hpp"

namespace elizaos {
namespace eliza_client {

bool Playwright::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Playwright::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Playwright::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

#include "enhance-prompt.hpp"

namespace elizaos {
namespace autofun_server {

bool EnhancePrompt::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void EnhancePrompt::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json EnhancePrompt::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

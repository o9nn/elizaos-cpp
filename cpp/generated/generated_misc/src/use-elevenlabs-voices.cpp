#include "use-elevenlabs-voices.hpp"

namespace elizaos {
namespace generated_misc {

bool UseElevenlabsVoices::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UseElevenlabsVoices::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UseElevenlabsVoices::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

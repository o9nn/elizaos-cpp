#include "voice-models.hpp"

namespace elizaos {
namespace generated_database {

bool VoiceModels::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void VoiceModels::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json VoiceModels::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_database
} // namespace elizaos

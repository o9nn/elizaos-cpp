#include "audio.hpp"

namespace elizaos {
namespace eliza_api_client {

bool Audio::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Audio::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Audio::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_api_client
} // namespace elizaos

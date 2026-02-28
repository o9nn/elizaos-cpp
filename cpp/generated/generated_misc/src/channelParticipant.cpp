#include "channelParticipant.hpp"

namespace elizaos {
namespace generated_misc {

bool Channelparticipant::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Channelparticipant::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Channelparticipant::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

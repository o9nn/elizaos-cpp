#include "message-types.hpp"

namespace elizaos {
namespace generated_misc {

bool MessageTypes::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void MessageTypes::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json MessageTypes::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

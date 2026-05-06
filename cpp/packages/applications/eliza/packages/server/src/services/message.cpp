#include "message.hpp"

namespace elizaos {
namespace eliza_server {

bool Message::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Message::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Message::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_server
} // namespace elizaos

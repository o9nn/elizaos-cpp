#include "logger.hpp"

namespace elizaos {
namespace autofun_server {

bool Logger::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Logger::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Logger::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

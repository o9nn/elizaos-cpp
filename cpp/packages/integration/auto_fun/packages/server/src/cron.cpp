#include "cron.hpp"

namespace elizaos {
namespace autofun_server {

bool Cron::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Cron::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Cron::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

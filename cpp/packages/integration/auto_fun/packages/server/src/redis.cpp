#include "redis.hpp"

namespace elizaos {
namespace autofun_server {

bool Redis::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Redis::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Redis::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_server
} // namespace elizaos

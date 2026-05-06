#include "health.hpp"

namespace elizaos {
namespace eliza_server {

bool Health::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Health::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Health::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_server
} // namespace elizaos

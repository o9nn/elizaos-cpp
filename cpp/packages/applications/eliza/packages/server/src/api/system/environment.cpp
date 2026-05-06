#include "environment.hpp"

namespace elizaos {
namespace eliza_server {

bool Environment::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Environment::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Environment::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_server
} // namespace elizaos

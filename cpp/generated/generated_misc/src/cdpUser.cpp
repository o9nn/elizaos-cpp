#include "cdpUser.hpp"

namespace elizaos {
namespace generated_misc {

bool Cdpuser::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Cdpuser::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Cdpuser::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

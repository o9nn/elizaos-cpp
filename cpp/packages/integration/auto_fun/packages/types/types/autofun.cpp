#include "autofun.hpp"

namespace elizaos {
namespace autofun_types {

bool Autofun::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Autofun::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Autofun::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_types
} // namespace elizaos

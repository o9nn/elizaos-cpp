#include "acceptAuthority.hpp"

namespace elizaos {
namespace autofun_program {

bool Acceptauthority::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Acceptauthority::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Acceptauthority::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_program
} // namespace elizaos

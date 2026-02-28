#include "checkConfig.hpp"

namespace elizaos {
namespace autofun_program {

bool Checkconfig::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Checkconfig::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Checkconfig::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_program
} // namespace elizaos

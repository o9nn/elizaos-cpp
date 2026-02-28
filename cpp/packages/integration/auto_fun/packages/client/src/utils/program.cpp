#include "program.hpp"

namespace elizaos {
namespace autofun_client {

bool Program::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Program::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Program::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

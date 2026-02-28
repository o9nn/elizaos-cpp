#include "deploy.hpp"

namespace elizaos {
namespace autofun_program {

bool Deploy::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Deploy::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Deploy::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_program
} // namespace elizaos

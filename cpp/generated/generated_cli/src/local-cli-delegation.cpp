#include "local-cli-delegation.hpp"

namespace elizaos {
namespace generated_cli {

bool LocalCliDelegation::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void LocalCliDelegation::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json LocalCliDelegation::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_cli
} // namespace elizaos

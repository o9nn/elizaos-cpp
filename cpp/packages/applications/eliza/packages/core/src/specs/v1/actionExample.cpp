#include "actionExample.hpp"

namespace elizaos {
namespace eliza_core {

bool Actionexample::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Actionexample::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Actionexample::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

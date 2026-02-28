#include "inspector-cli.hpp"

namespace elizaos {
namespace generated_cli {

bool InspectorCli::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void InspectorCli::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json InspectorCli::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_cli
} // namespace elizaos

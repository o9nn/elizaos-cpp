#include "ApiIntegration.hpp"

namespace elizaos {
namespace eliza_project_starter {

bool Apiintegration::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Apiintegration::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Apiintegration::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_project_starter
} // namespace elizaos

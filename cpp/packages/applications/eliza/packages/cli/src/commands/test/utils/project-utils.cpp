#include "project-utils.hpp"

namespace elizaos {
namespace eliza_cli {

bool ProjectUtils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ProjectUtils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ProjectUtils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

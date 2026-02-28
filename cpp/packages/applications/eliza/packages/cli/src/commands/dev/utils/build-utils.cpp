#include "build-utils.hpp"

namespace elizaos {
namespace eliza_cli {

bool BuildUtils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void BuildUtils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json BuildUtils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

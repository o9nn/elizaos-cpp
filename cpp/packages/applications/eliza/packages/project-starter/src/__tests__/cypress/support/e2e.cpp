#include "e2e.hpp"

namespace elizaos {
namespace eliza_project_starter {

bool E2e::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void E2e::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json E2e::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_project_starter
} // namespace elizaos

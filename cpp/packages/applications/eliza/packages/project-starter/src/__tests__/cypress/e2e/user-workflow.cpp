#include "user-workflow.hpp"

namespace elizaos {
namespace eliza_project_starter {

bool UserWorkflow::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void UserWorkflow::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json UserWorkflow::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_project_starter
} // namespace elizaos

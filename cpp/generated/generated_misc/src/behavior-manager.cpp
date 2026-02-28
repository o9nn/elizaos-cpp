#include "behavior-manager.hpp"

namespace elizaos {
namespace generated_misc {

bool BehaviorManager::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void BehaviorManager::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json BehaviorManager::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

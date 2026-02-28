#include "skeleton.hpp"

namespace elizaos {
namespace eliza_client {

bool Skeleton::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Skeleton::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Skeleton::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

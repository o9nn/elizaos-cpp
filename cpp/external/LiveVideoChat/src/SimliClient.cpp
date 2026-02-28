#include "SimliClient.hpp"

namespace elizaos {
namespace LiveVideoChat {

bool Simliclient::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Simliclient::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Simliclient::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace LiveVideoChat
} // namespace elizaos

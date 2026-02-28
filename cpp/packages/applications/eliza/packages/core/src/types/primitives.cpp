#include "primitives.hpp"

namespace elizaos {
namespace eliza_core {

bool Primitives::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Primitives::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Primitives::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

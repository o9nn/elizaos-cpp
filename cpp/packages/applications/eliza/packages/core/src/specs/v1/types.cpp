#include "types.hpp"

namespace elizaos {
namespace eliza_core {

bool Types::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Types::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Types::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

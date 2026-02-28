#include "checkbox.hpp"

namespace elizaos {
namespace eliza_client {

bool Checkbox::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Checkbox::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Checkbox::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

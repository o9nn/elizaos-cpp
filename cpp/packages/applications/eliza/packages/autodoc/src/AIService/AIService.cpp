#include "AIService.hpp"

namespace elizaos {
namespace eliza_autodoc {

bool Aiservice::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Aiservice::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Aiservice::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_autodoc
} // namespace elizaos

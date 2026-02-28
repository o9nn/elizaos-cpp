#include "tailwind.hpp"

namespace elizaos {
namespace eliza_client {

bool Tailwind::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Tailwind::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Tailwind::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

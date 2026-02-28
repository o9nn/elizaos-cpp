#include "posts.hpp"

namespace elizaos {
namespace eliza_core {

bool Posts::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Posts::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Posts::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

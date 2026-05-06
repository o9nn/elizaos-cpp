#include "env.hpp"

namespace elizaos {
namespace generated_misc {

bool Env::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Env::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Env::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

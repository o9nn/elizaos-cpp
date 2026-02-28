#include "florence2-local.hpp"

namespace elizaos {
namespace generated_misc {

bool Florence2Local::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Florence2Local::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Florence2Local::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

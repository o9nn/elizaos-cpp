#include "app-types.hpp"

namespace elizaos {
namespace generated_misc {

bool AppTypes::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void AppTypes::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json AppTypes::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

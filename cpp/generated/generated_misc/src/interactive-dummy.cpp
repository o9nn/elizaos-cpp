#include "interactive-dummy.hpp"

namespace elizaos {
namespace generated_misc {

bool InteractiveDummy::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void InteractiveDummy::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json InteractiveDummy::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

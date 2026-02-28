#include "tauri-utils.hpp"

namespace elizaos {
namespace generated_utils {

bool TauriUtils::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void TauriUtils::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json TauriUtils::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_utils
} // namespace elizaos

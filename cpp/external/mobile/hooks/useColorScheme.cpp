#include "useColorScheme.hpp"

namespace elizaos {
namespace mobile {

bool Usecolorscheme::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Usecolorscheme::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Usecolorscheme::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace mobile
} // namespace elizaos

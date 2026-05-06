#include "tsup.hpp"

namespace elizaos {
namespace eliza_cli {

bool Tsup::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Tsup::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Tsup::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

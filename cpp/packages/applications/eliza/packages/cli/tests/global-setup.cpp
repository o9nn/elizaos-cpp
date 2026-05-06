#include "global-setup.hpp"

namespace elizaos {
namespace eliza_cli {

bool GlobalSetup::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void GlobalSetup::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json GlobalSetup::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

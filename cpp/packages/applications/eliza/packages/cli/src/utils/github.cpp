#include "github.hpp"

namespace elizaos {
namespace eliza_cli {

bool Github::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Github::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Github::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

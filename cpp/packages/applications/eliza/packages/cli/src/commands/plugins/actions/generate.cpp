#include "generate.hpp"

namespace elizaos {
namespace eliza_cli {

bool Generate::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Generate::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Generate::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

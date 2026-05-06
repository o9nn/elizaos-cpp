#include "cli-prompts.hpp"

namespace elizaos {
namespace generated_cli {

bool CliPrompts::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void CliPrompts::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json CliPrompts::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_cli
} // namespace elizaos

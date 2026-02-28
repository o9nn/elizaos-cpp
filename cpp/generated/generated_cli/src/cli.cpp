#include "cli.hpp"

namespace elizaos {
namespace generated_cli {

bool Cli::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Cli::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Cli::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_cli
} // namespace elizaos

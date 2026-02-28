#include "eliza.hpp"

namespace elizaos {
namespace eliza_cli {

bool Eliza::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Eliza::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Eliza::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

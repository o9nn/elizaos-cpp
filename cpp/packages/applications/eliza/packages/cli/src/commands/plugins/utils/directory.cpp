#include "directory.hpp"

namespace elizaos {
namespace eliza_cli {

bool Directory::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Directory::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Directory::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

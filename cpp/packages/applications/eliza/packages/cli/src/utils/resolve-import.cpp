#include "resolve-import.hpp"

namespace elizaos {
namespace eliza_cli {

bool ResolveImport::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ResolveImport::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ResolveImport::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

#include "DirectoryTraversal.hpp"

namespace elizaos {
namespace eliza_autodoc {

bool Directorytraversal::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Directorytraversal::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Directorytraversal::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_autodoc
} // namespace elizaos

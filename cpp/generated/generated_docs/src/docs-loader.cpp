#include "docs-loader.hpp"

namespace elizaos {
namespace generated_docs {

bool DocsLoader::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void DocsLoader::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json DocsLoader::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_docs
} // namespace elizaos

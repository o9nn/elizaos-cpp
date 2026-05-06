#include "CodeFormatter.hpp"

namespace elizaos {
namespace eliza_autodoc {

bool Codeformatter::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Codeformatter::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Codeformatter::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_autodoc
} // namespace elizaos

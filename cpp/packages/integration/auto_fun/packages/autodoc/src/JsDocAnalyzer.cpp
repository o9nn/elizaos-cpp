#include "JsDocAnalyzer.hpp"

namespace elizaos {
namespace autofun_autodoc {

bool Jsdocanalyzer::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Jsdocanalyzer::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Jsdocanalyzer::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_autodoc
} // namespace elizaos

#include "JsDocGenerator.hpp"

namespace elizaos {
namespace autofun_autodoc {

bool Jsdocgenerator::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Jsdocgenerator::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Jsdocgenerator::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_autodoc
} // namespace elizaos

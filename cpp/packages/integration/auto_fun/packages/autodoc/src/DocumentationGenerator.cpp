#include "DocumentationGenerator.hpp"

namespace elizaos {
namespace autofun_autodoc {

bool Documentationgenerator::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Documentationgenerator::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Documentationgenerator::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_autodoc
} // namespace elizaos

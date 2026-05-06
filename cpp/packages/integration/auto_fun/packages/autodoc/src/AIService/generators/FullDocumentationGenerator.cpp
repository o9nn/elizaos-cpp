#include "FullDocumentationGenerator.hpp"

namespace elizaos {
namespace autofun_autodoc {

bool Fulldocumentationgenerator::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Fulldocumentationgenerator::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Fulldocumentationgenerator::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_autodoc
} // namespace elizaos

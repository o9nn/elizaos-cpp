#include "model.hpp"

namespace elizaos {
namespace eliza_core {

bool Model::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Model::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Model::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_core
} // namespace elizaos

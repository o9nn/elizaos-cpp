#include "florence2-model.hpp"

namespace elizaos {
namespace generated_database {

bool Florence2Model::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Florence2Model::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Florence2Model::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_database
} // namespace elizaos

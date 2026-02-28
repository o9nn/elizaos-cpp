#include "parsing.hpp"

namespace elizaos {
namespace generated_misc {

bool Parsing::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Parsing::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Parsing::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

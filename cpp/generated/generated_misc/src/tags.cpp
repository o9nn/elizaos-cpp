#include "tags.hpp"

namespace elizaos {
namespace generated_misc {

bool Tags::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Tags::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Tags::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

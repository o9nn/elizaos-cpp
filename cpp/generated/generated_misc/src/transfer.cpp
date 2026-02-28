#include "transfer.hpp"

namespace elizaos {
namespace generated_misc {

bool Transfer::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Transfer::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Transfer::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_misc
} // namespace elizaos

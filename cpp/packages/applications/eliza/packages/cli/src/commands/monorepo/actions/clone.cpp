#include "clone.hpp"

namespace elizaos {
namespace eliza_cli {

bool Clone::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void Clone::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json Clone::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

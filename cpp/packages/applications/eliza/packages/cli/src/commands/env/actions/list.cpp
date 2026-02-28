#include "list.hpp"

namespace elizaos {
namespace eliza_cli {

bool List::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void List::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json List::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

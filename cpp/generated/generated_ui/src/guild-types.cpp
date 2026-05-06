#include "guild-types.hpp"

namespace elizaos {
namespace generated_ui {

bool GuildTypes::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void GuildTypes::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json GuildTypes::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace generated_ui
} // namespace elizaos

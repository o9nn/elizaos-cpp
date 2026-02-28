#include "search-bar.hpp"

namespace elizaos {
namespace autofun_client {

bool SearchBar::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void SearchBar::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json SearchBar::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace autofun_client
} // namespace elizaos

#include "emoji-handler.hpp"

namespace elizaos {
namespace eliza_cli {

bool EmojiHandler::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void EmojiHandler::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json EmojiHandler::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_cli
} // namespace elizaos

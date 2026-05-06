#include "chat-message-list.hpp"

namespace elizaos {
namespace eliza_client {

bool ChatMessageList::initialize(const nlohmann::json& config) {
    if (initialized_) return true;
    config_ = config;
    initialized_ = true;
    return true;
}

void ChatMessageList::shutdown() {
    initialized_ = false;
    config_ = {};
}

nlohmann::json ChatMessageList::getStatus() const {
    nlohmann::json status;
    status["name"] = getName();
    status["initialized"] = initialized_;
    return status;
}

} // namespace eliza_client
} // namespace elizaos

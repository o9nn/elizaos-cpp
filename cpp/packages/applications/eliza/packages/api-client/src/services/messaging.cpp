#include "messaging.hpp"

#include <utility>

namespace elizaos {
namespace eliza_api_client {

namespace {
std::string pathFromConfig(const nlohmann::json& config,
                           const std::string& camelKey,
                           const std::string& snakeKey,
                           const std::string& defaultValue) {
    const auto camelIt = config.find(camelKey);
    if (camelIt != config.end() && camelIt->is_string() && !camelIt->get<std::string>().empty()) {
        return camelIt->get<std::string>();
    }
    const auto snakeIt = config.find(snakeKey);
    if (snakeIt != config.end() && snakeIt->is_string() && !snakeIt->get<std::string>().empty()) {
        return snakeIt->get<std::string>();
    }
    return defaultValue;
}

void trimTrailingSlash(std::string& path) {
    while (path.size() > 1 && path.back() == '/') {
        path.pop_back();
    }
}

bool containsTextOrPayload(const nlohmann::json& value) {
    const auto text = value.find("text");
    const auto content = value.find("content");
    const auto payload = value.find("payload");
    return (text != value.end() && text->is_string() && !text->get<std::string>().empty()) ||
           (content != value.end() && !content->is_null() && !content->empty()) ||
           (payload != value.end() && !payload->is_null() && !payload->empty());
}
} // namespace

Messaging::Messaging(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool Messaging::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }
    if (!config.is_object()) {
        setLastError("invalid_config", "Messaging service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    messagesPath_ = pathFromConfig(config_, "messagesPath", "messages_path", "/api/messages");
    conversationsPath_ = pathFromConfig(config_, "conversationsPath", "conversations_path", "/api/conversations");
    trimTrailingSlash(messagesPath_);
    trimTrailingSlash(conversationsPath_);
    config_["messagesPath"] = messagesPath_;
    config_["conversationsPath"] = conversationsPath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }
    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "Messaging service could not initialize its BaseClient." : client_->getLastErrorMessage());
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void Messaging::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    messagesPath_ = "/api/messages";
    conversationsPath_ = "/api/conversations";
    clearLastError();
}

void Messaging::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult Messaging::sendMessage(const nlohmann::json& message) const {
    if (!initialized_) {
        return ApiResult::failure("messaging_not_initialized", "Messaging service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Messaging service has no BaseClient configured.");
    }
    if (!message.is_object() || !containsTextOrPayload(message)) {
        return ApiResult::failure("invalid_message", "Message payload requires non-empty text, content, or payload.");
    }
    return client_->request("POST", messagesPath_, message);
}

ApiResult Messaging::listMessages() const {
    if (!initialized_) {
        return ApiResult::failure("messaging_not_initialized", "Messaging service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Messaging service has no BaseClient configured.");
    }
    return client_->request("GET", messagesPath_);
}

ApiResult Messaging::getConversation(const std::string& conversationId) const {
    if (!initialized_) {
        return ApiResult::failure("messaging_not_initialized", "Messaging service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Messaging service has no BaseClient configured.");
    }
    if (conversationId.empty()) {
        return ApiResult::failure("missing_conversation_id", "Conversation ID must not be empty.");
    }
    return client_->request("GET", conversationPath(conversationId));
}

ApiResult Messaging::markMessageRead(const std::string& messageId) const {
    if (!initialized_) {
        return ApiResult::failure("messaging_not_initialized", "Messaging service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Messaging service has no BaseClient configured.");
    }
    if (messageId.empty()) {
        return ApiResult::failure("missing_message_id", "Message ID must not be empty.");
    }
    return client_->request("POST", messagePath(messageId) + "/read", nlohmann::json::object());
}

ApiResult Messaging::deleteMessage(const std::string& messageId) const {
    if (!initialized_) {
        return ApiResult::failure("messaging_not_initialized", "Messaging service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Messaging service has no BaseClient configured.");
    }
    if (messageId.empty()) {
        return ApiResult::failure("missing_message_id", "Message ID must not be empty.");
    }
    return client_->request("DELETE", messagePath(messageId));
}

nlohmann::json Messaging::getStatus() const {
    return {
        {"name", getName()},
        {"initialized", initialized_},
        {"messagesPath", messagesPath_},
        {"conversationsPath", conversationsPath_},
        {"clientConfigured", static_cast<bool>(client_)},
        {"client", client_ ? client_->getStatus() : nlohmann::json::object()},
        {"lastErrorCode", lastErrorCode_},
        {"lastErrorMessage", lastErrorMessage_}
    };
}

void Messaging::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Messaging::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

std::string Messaging::messagePath(const std::string& messageId) const {
    return messagesPath_ + "/" + messageId;
}

std::string Messaging::conversationPath(const std::string& conversationId) const {
    return conversationsPath_ + "/" + conversationId;
}

} // namespace eliza_api_client
} // namespace elizaos

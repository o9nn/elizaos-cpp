#include "memory.hpp"

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

bool hasNonEmptyQuery(const nlohmann::json& value) {
    const auto text = value.find("query");
    const auto textSnake = value.find("query_text");
    const auto embedding = value.find("embedding");
    return (text != value.end() && text->is_string() && !text->get<std::string>().empty()) ||
           (textSnake != value.end() && textSnake->is_string() && !textSnake->get<std::string>().empty()) ||
           (embedding != value.end() && embedding->is_array() && !embedding->empty());
}
} // namespace

Memory::Memory(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool Memory::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }
    if (!config.is_object()) {
        setLastError("invalid_config", "Memory service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    basePath_ = pathFromConfig(config_, "memoryPath", "memory_path", "/api/memory");
    trimTrailingSlash(basePath_);
    searchPath_ = pathFromConfig(config_, "memorySearchPath", "memory_search_path", basePath_ + "/search");
    trimTrailingSlash(searchPath_);
    config_["memoryPath"] = basePath_;
    config_["memorySearchPath"] = searchPath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }
    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "Memory service could not initialize its BaseClient." : client_->getLastErrorMessage());
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void Memory::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    basePath_ = "/api/memory";
    searchPath_ = "/api/memory/search";
    clearLastError();
}

void Memory::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult Memory::listMemories() const {
    if (!initialized_) {
        return ApiResult::failure("memory_not_initialized", "Memory service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Memory service has no BaseClient configured.");
    }
    return client_->request("GET", basePath_);
}

ApiResult Memory::getMemory(const std::string& memoryId) const {
    if (!initialized_) {
        return ApiResult::failure("memory_not_initialized", "Memory service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Memory service has no BaseClient configured.");
    }
    if (memoryId.empty()) {
        return ApiResult::failure("missing_memory_id", "Memory ID must not be empty.");
    }
    return client_->request("GET", memoryPath(memoryId));
}

ApiResult Memory::createMemory(const nlohmann::json& memory) const {
    if (!initialized_) {
        return ApiResult::failure("memory_not_initialized", "Memory service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Memory service has no BaseClient configured.");
    }
    if (!memory.is_object() || memory.empty()) {
        return ApiResult::failure("invalid_memory", "Memory creation payload must be a non-empty JSON object.");
    }
    return client_->request("POST", basePath_, memory);
}

ApiResult Memory::searchMemories(const nlohmann::json& query) const {
    if (!initialized_) {
        return ApiResult::failure("memory_not_initialized", "Memory service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Memory service has no BaseClient configured.");
    }
    if (!query.is_object() || !hasNonEmptyQuery(query)) {
        return ApiResult::failure("invalid_memory_query", "Memory search query requires non-empty query, query_text, or embedding.");
    }
    return client_->request("POST", searchPath_, query);
}

ApiResult Memory::deleteMemory(const std::string& memoryId) const {
    if (!initialized_) {
        return ApiResult::failure("memory_not_initialized", "Memory service must be initialized before requests can be sent.");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Memory service has no BaseClient configured.");
    }
    if (memoryId.empty()) {
        return ApiResult::failure("missing_memory_id", "Memory ID must not be empty.");
    }
    return client_->request("DELETE", memoryPath(memoryId));
}

nlohmann::json Memory::getStatus() const {
    return {
        {"name", getName()},
        {"initialized", initialized_},
        {"memoryPath", basePath_},
        {"memorySearchPath", searchPath_},
        {"clientConfigured", static_cast<bool>(client_)},
        {"client", client_ ? client_->getStatus() : nlohmann::json::object()},
        {"lastErrorCode", lastErrorCode_},
        {"lastErrorMessage", lastErrorMessage_}
    };
}

void Memory::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Memory::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

std::string Memory::memoryPath(const std::string& memoryId) const {
    return basePath_ + "/" + memoryId;
}

} // namespace eliza_api_client
} // namespace elizaos

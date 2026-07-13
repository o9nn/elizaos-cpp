#include "media.hpp"

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

ApiResult notInitialized(const std::string& service) {
    return ApiResult::failure(service + "_not_initialized", service + " service must be initialized before requests can be sent.");
}

bool blank(const std::string& value) {
    return value.empty();
}
} // namespace

Media::Media(std::shared_ptr<BaseClient> client)
    : client_(std::move(client)) {}

bool Media::initialize(const nlohmann::json& config) {
    if (initialized_) {
        return true;
    }
    if (!config.is_object()) {
        setLastError("invalid_config", "Media service configuration must be a JSON object.");
        return false;
    }

    config_ = config;
    basePath_ = pathFromConfig(config_, "mediaPath", "media_path", "/api/media");
    while (basePath_.size() > 1 && basePath_.back() == '/') {
        basePath_.pop_back();
    }
    config_["mediaPath"] = basePath_;

    if (!client_) {
        client_ = std::make_shared<BaseClient>();
    }
    if (!client_->isInitialized()) {
        if (!client_->initialize(config_)) {
            setLastError(client_->getLastErrorCode().empty() ? "client_initialization_failed" : client_->getLastErrorCode(),
                         client_->getLastErrorMessage().empty() ? "Media service could not initialize its BaseClient." : client_->getLastErrorMessage());
            return false;
        }
    }

    initialized_ = true;
    clearLastError();
    return true;
}

void Media::shutdown() {
    initialized_ = false;
    config_ = nlohmann::json::object();
    basePath_ = "/api/media";
    clearLastError();
}

void Media::setClient(std::shared_ptr<BaseClient> client) {
    client_ = std::move(client);
}

ApiResult Media::uploadMedia(const nlohmann::json& descriptor) const {
    if (!initialized_) {
        return notInitialized("media");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Media service has no BaseClient configured.");
    }
    if (!descriptor.is_object()) {
        return ApiResult::failure("invalid_media_descriptor", "Media upload descriptor must be a JSON object.");
    }
    if (descriptor.empty()) {
        return ApiResult::failure("empty_media_descriptor", "Media upload descriptor must not be empty.");
    }
    return client_->request("POST", basePath_, descriptor);
}

ApiResult Media::listMedia() const {
    if (!initialized_) {
        return notInitialized("media");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Media service has no BaseClient configured.");
    }
    return client_->request("GET", basePath_);
}

ApiResult Media::getMedia(const std::string& mediaId) const {
    if (!initialized_) {
        return notInitialized("media");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Media service has no BaseClient configured.");
    }
    if (blank(mediaId)) {
        return ApiResult::failure("missing_media_id", "Media ID must not be empty.");
    }
    return client_->request("GET", mediaPath(mediaId));
}

ApiResult Media::deleteMedia(const std::string& mediaId) const {
    if (!initialized_) {
        return notInitialized("media");
    }
    if (!client_) {
        return ApiResult::failure("client_missing", "Media service has no BaseClient configured.");
    }
    if (blank(mediaId)) {
        return ApiResult::failure("missing_media_id", "Media ID must not be empty.");
    }
    return client_->request("DELETE", mediaPath(mediaId));
}

nlohmann::json Media::getStatus() const {
    return {
        {"name", getName()},
        {"initialized", initialized_},
        {"mediaPath", basePath_},
        {"clientConfigured", static_cast<bool>(client_)},
        {"client", client_ ? client_->getStatus() : nlohmann::json::object()},
        {"lastErrorCode", lastErrorCode_},
        {"lastErrorMessage", lastErrorMessage_}
    };
}

void Media::setLastError(std::string code, std::string message) {
    lastErrorCode_ = std::move(code);
    lastErrorMessage_ = std::move(message);
}

void Media::clearLastError() {
    lastErrorCode_.clear();
    lastErrorMessage_.clear();
}

std::string Media::mediaPath(const std::string& mediaId) const {
    return basePath_ + "/" + mediaId;
}

} // namespace eliza_api_client
} // namespace elizaos

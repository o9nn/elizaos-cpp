#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MESSAGING_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MESSAGING_HPP_

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../lib/base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

class Messaging {
public:
    Messaging() = default;
    explicit Messaging(std::shared_ptr<BaseClient> client);
    ~Messaging() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    ApiResult sendMessage(const nlohmann::json& message) const;
    ApiResult listMessages() const;
    ApiResult getConversation(const std::string& conversationId) const;
    ApiResult markMessageRead(const std::string& messageId) const;
    ApiResult deleteMessage(const std::string& messageId) const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "messaging"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();
    std::string messagePath(const std::string& messageId) const;
    std::string conversationPath(const std::string& conversationId) const;

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string messagesPath_ = "/api/messages";
    std::string conversationsPath_ = "/api/conversations";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MESSAGING_HPP_

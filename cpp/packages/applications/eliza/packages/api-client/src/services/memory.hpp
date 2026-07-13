#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MEMORY_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MEMORY_HPP_

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../lib/base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

class Memory {
public:
    Memory() = default;
    explicit Memory(std::shared_ptr<BaseClient> client);
    ~Memory() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    ApiResult listMemories() const;
    ApiResult getMemory(const std::string& memoryId) const;
    ApiResult createMemory(const nlohmann::json& memory) const;
    ApiResult searchMemories(const nlohmann::json& query) const;
    ApiResult deleteMemory(const std::string& memoryId) const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "memory"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();
    std::string memoryPath(const std::string& memoryId) const;

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string basePath_ = "/api/memory";
    std::string searchPath_ = "/api/memory/search";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_MEMORY_HPP_

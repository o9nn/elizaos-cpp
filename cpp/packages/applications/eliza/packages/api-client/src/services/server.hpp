#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_SERVER_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_SERVER_HPP_

#include <memory>
#include <string>

#include <nlohmann/json.hpp>

#include "../lib/base-client.hpp"

namespace elizaos {
namespace eliza_api_client {

class Server {
public:
    Server() = default;
    explicit Server(std::shared_ptr<BaseClient> client);
    ~Server() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();

    void setClient(std::shared_ptr<BaseClient> client);
    std::shared_ptr<BaseClient> getClient() const { return client_; }

    ApiResult getHealth() const;
    ApiResult getServerStatus() const;

    nlohmann::json getStatus() const;
    std::string getName() const { return "server"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }
    const std::string& getLastErrorCode() const { return lastErrorCode_; }
    const std::string& getLastErrorMessage() const { return lastErrorMessage_; }

private:
    void setLastError(std::string code, std::string message);
    void clearLastError();

    nlohmann::json config_ = nlohmann::json::object();
    bool initialized_ = false;
    std::string healthPath_ = "/api/server/health";
    std::string statusPath_ = "/api/server/status";
    std::string lastErrorCode_;
    std::string lastErrorMessage_;
    std::shared_ptr<BaseClient> client_;
};

} // namespace eliza_api_client
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_API_CLIENT_SRC_SERVICES_SERVER_HPP_

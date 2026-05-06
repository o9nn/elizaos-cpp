#ifndef ELIZAOS_CPP_GENERATED_GENERATED_CLI_INCLUDE_WEBSOCKET_CLIENT_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_CLI_INCLUDE_WEBSOCKET_CLIENT_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_cli {

class WebsocketClient {
public:
    WebsocketClient() = default;
    ~WebsocketClient() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "websocket_client"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_cli
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_CLI_INCLUDE_WEBSOCKET_CLIENT_HPP_

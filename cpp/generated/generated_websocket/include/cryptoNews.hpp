#ifndef ELIZAOS_CPP_GENERATED_GENERATED_WEBSOCKET_INCLUDE_CRYPTONEWS_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_WEBSOCKET_INCLUDE_CRYPTONEWS_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_websocket {

class Cryptonews {
public:
    Cryptonews() = default;
    ~Cryptonews() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "cryptoNews"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_websocket
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_WEBSOCKET_INCLUDE_CRYPTONEWS_HPP_

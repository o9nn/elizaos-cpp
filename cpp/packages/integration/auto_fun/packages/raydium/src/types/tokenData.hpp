#ifndef ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_RAYDIUM_SRC_TYPES_TOKENDATA_HPP_
#define ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_RAYDIUM_SRC_TYPES_TOKENDATA_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace autofun_raydium {

class Tokendata {
public:
    Tokendata() = default;
    ~Tokendata() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "tokenData"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace autofun_raydium
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_RAYDIUM_SRC_TYPES_TOKENDATA_HPP_

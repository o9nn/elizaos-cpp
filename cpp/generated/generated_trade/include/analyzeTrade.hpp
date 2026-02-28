#ifndef ELIZAOS_CPP_GENERATED_GENERATED_TRADE_INCLUDE_ANALYZETRADE_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_TRADE_INCLUDE_ANALYZETRADE_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_trade {

class Analyzetrade {
public:
    Analyzetrade() = default;
    ~Analyzetrade() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "analyzeTrade"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_trade
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_TRADE_INCLUDE_ANALYZETRADE_HPP_

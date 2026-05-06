#ifndef ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_BASETRADESERVICE_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_BASETRADESERVICE_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_services {

class Basetradeservice {
public:
    Basetradeservice() = default;
    ~Basetradeservice() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "BaseTradeService"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_services
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_BASETRADESERVICE_HPP_

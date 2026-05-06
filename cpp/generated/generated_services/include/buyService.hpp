#ifndef ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_BUYSERVICE_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_BUYSERVICE_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_services {

class Buyservice {
public:
    Buyservice() = default;
    ~Buyservice() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "buyService"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_services
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_BUYSERVICE_HPP_

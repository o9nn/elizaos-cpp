#ifndef ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_SERVICE_INTERFACES_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_SERVICE_INTERFACES_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_services {

class ServiceInterfaces {
public:
    ServiceInterfaces() = default;
    ~ServiceInterfaces() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "service_interfaces"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_services
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_SERVICES_INCLUDE_SERVICE_INTERFACES_HPP_

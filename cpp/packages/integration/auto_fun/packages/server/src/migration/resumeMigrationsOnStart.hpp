#ifndef ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_SERVER_SRC_MIGRATION_RESUMEMIGRATIONSONSTART_HPP_
#define ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_SERVER_SRC_MIGRATION_RESUMEMIGRATIONSONSTART_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace autofun_server {

class Resumemigrationsonstart {
public:
    Resumemigrationsonstart() = default;
    ~Resumemigrationsonstart() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "resumeMigrationsOnStart"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace autofun_server
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_INTEGRATION_AUTO_FUN_PACKAGES_SERVER_SRC_MIGRATION_RESUMEMIGRATIONSONSTART_HPP_

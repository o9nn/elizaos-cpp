#ifndef ELIZAOS_CPP_GENERATED_GENERATED_MISC_INCLUDE_USE_ADMIN_USERS_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_MISC_INCLUDE_USE_ADMIN_USERS_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_misc {

class UseAdminUsers {
public:
    UseAdminUsers() = default;
    ~UseAdminUsers() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "use_admin_users"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_misc
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_MISC_INCLUDE_USE_ADMIN_USERS_HPP_

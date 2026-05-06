#ifndef ELIZAOS_CPP_GENERATED_GENERATED_UTILS_INCLUDE_DIRECTORY_UTILS_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_UTILS_INCLUDE_DIRECTORY_UTILS_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_utils {

class DirectoryUtils {
public:
    DirectoryUtils() = default;
    ~DirectoryUtils() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "directory_utils"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_utils
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_UTILS_INCLUDE_DIRECTORY_UTILS_HPP_

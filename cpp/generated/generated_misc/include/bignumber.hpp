#ifndef ELIZAOS_CPP_GENERATED_GENERATED_MISC_INCLUDE_BIGNUMBER_HPP_
#define ELIZAOS_CPP_GENERATED_GENERATED_MISC_INCLUDE_BIGNUMBER_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace generated_misc {

class Bignumber {
public:
    Bignumber() = default;
    ~Bignumber() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "bignumber"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace generated_misc
} // namespace elizaos

#endif // ELIZAOS_CPP_GENERATED_GENERATED_MISC_INCLUDE_BIGNUMBER_HPP_

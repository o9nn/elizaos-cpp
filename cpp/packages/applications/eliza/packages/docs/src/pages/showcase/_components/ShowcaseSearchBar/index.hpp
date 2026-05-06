#ifndef ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_DOCS_SRC_PAGES_SHOWCASE__COMPONENTS_SHOWCASESEARCHBAR_INDEX_HPP_
#define ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_DOCS_SRC_PAGES_SHOWCASE__COMPONENTS_SHOWCASESEARCHBAR_INDEX_HPP_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <optional>
#include <nlohmann/json.hpp>

namespace elizaos {
namespace eliza_docs {

class Index {
public:
    Index() = default;
    ~Index() = default;

    bool initialize(const nlohmann::json& config = {});
    void shutdown();
    nlohmann::json getStatus() const;
    std::string getName() const { return "index"; }
    bool isInitialized() const { return initialized_; }
    const nlohmann::json& getConfig() const { return config_; }

private:
    nlohmann::json config_;
    bool initialized_ = false;
};

} // namespace eliza_docs
} // namespace elizaos

#endif // ELIZAOS_CPP_PACKAGES_APPLICATIONS_ELIZA_PACKAGES_DOCS_SRC_PAGES_SHOWCASE__COMPONENTS_SHOWCASESEARCHBAR_INDEX_HPP_

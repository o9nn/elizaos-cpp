#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TestTimeoutManager {
public:
    TestTimeoutManager getInstance();
    void startTimeout(const std::string& testName, number = 30000 duration);
    void clearTimeout(const std::string& testName);
    void clearAll();
};

// Export singleton

} // namespace elizaos

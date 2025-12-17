#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TestTimeoutManager {
public:
    static TestTimeoutManager getInstance();
    void startTimeout(const std::string& testName, double duration = 30000);
    void clearTimeout(const std::string& testName);
    void clearAll();
};

// Export singleton

} // namespace elizaos

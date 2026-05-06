#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "logger.js.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class PlaywrightInstaller {
public:
    PlaywrightInstaller(Logger logger);
    bool isPlaywrightInstalled();
    std::future<void> installPlaywright();
    std::future<void> ensurePlaywrightInstalled();
    bool isReady();

private:
    Logger logger_;
};


} // namespace elizaos

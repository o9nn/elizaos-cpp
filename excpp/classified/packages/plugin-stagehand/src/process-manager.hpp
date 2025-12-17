#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class StagehandProcessManager {
public:
    StagehandProcessManager(double serverPort = 3456);
     getBinaryName();
    std::string findBinary();
    std::future<void> start();
    std::future<void> waitForServer();
    std::future<void> stop();
    bool isServerRunning();
    std::string getServerUrl();

private:
    double serverPort_;
};


} // namespace elizaos

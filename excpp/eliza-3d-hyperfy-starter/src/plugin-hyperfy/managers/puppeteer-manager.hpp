#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".service.js.hpp"
#include ".utils.js.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

//@ts-nocheck

class PuppeteerManager {
public:
    PuppeteerManager(IAgentRuntime runtime);
    static PuppeteerManager getInstance();
    void init();
    void injectScripts(const std::vector<std::string>& scriptPaths);
    std::future<std::string> snapshotFacingDirection(const std::string& direction);
    std::future<std::string> snapshotViewToTarget([number targetPosition, auto number);
    std::future<std::string> snapshotEquirectangular();
    std::future<std::vector<double>> loadGlbBytes(const std::string& url);
    std::future<std::vector<double>> loadVRMBytes(const std::string& url);
    std::future<std::string> registerTexture(const std::string& url, const std::string& slot);
    std::future<void> loadEnvironmentHDR(const std::string& url);
    void rehydrateSceneAssets();
    void getService();
};


} // namespace elizaos

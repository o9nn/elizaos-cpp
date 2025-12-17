#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "hyperfy/src/core/createNodeClientWorld.js.hpp"
#include "managers//emote-manager.js.hpp"
#include "managers//message-manager.js.hpp"
#include "managers//voice-manager.js.hpp"
#include "managers/behavior-manager.js.hpp"
#include "managers/build-manager.js.hpp"
#include "managers/puppeteer-manager.js.hpp"
#include "physx/loadPhysX.js.hpp"
#include "systems/actions.js.hpp"
#include "systems/controls.hpp"
#include "systems/environment.js.hpp"
#include "systems/liveKit.js.hpp"
#include "systems/loader.hpp"
#include "utils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class HyperfyService {
public:
    HyperfyService();
    std::optional<UUID> currentWorldId();
    std::optional<std::any> getWorld();
    static std::future<HyperfyService> start(IAgentRuntime runtime);
    static std::future<void> stop(IAgentRuntime runtime);
    std::future<void> connect(std::optional<std::any> config);
    void subscribeToHyperfyEvents();
    Promise< uploadCharacterAssets();
    void startAppearancePolling();
    void stopAppearancePolling();
    bool isConnected();
    std::optional<std::any> getEntityById(const std::string& entityId);
    std::string getEntityName(const std::string& entityId);
    std::future<void> handleDisconnect();
    std::future<void> disconnect();
    std::future<void> changeName(const std::string& newName);
    void startChatSubscription();
    void getEmoteManager();
    void getBehaviorManager();
    void getMessageManager();
    void getVoiceManager();
    void getPuppeteerManager();
    void getBuildManager();

private:
    BehaviorManager behaviorManager_;
    EmoteManager emoteManager_;
    MessageManager messageManager_;
    VoiceManager voiceManager_;
    PuppeteerManager puppeteerManager_;
    BuildManager buildManager_;
};


} // namespace elizaos

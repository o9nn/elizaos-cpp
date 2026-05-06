#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".constants.js.hpp"
#include ".hyperfy/src/core/extras/playerEmotes.js.hpp"
#include ".service.js.hpp"
#include ".utils.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class EmoteManager {
public:
    EmoteManager(auto runtime);
    void uploadEmotes();
    void playEmote(const std::string& name);
    void clearEmote(auto player);
    void clearTimers();
    void getService();

private:
    IAgentRuntime runtime_;
};


} // namespace elizaos

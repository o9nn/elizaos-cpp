#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".events.hpp"
#include ".hyperfy/src/core/utils.hpp"
#include ".service.hpp"
#include "elizaos/core.hpp"
#include "guards.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class MessageManager {
public:
    MessageManager(IAgentRuntime runtime);
    std::future<void> handleMessage(auto msg);
    std::future<void> sendMessage(const std::string& text);
    void formatMessages(auto {
    messages, auto entities, const std::any& });
    void getRecentMessages(UUID roomId, auto count = 20);
    void getService();


} // namespace elizaos

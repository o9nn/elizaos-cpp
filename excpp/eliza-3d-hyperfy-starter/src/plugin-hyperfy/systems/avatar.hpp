#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "hyperfy/src/core/nodes/Node.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using AvatarFactory = std::function<void()>;
    void commit(bool didMove);
    void unmount();
    void applyStats(const std::any& stats);
    std::string src() const;
    std::string emote() const;
    std::optional<double> getHeight();
    std::optional<double> getHeadToHeight();
    void setEmote(const std::string& url);
    std::optional<double> height() const;
    this copy(AgentAvatar source, bool recursive);
    std::any getProxy();

} // namespace elizaos

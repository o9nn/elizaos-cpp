#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".hyperfy/src/core/nodes/Node.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



using AvatarFactory = std::variant<std::function<void()>, std::function<void()>>;
    void commit(bool didMove);
    void unmount();
    void applyStats(const std::any& stats);
    std::optional<std::string> src() const;
    void src(const std::optional<std::string>& value);
    std::optional<std::string> emote() const;
    void emote(const std::optional<std::string>& value);
    std::optional<double> getHeight();
    std::optional<double> getHeadToHeight();
    void setEmote(const std::optional<std::string>& url);
    std::optional<double> height() const;
    this copy(AgentAvatar source, bool recursive);
    std::any getProxy();

} // namespace elizaos

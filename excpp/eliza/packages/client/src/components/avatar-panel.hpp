#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct AvatarPanelProps {
    Agent characterValue;
    { setCharacterValue;
    std::optional<(avatarUrl: string) => void> updateAvatar;
    std::optional<<T>(path: string, value: T) => void> updateSetting;
    std::optional<<T>(path: string, value: T) => void> updateField;
};


} // namespace elizaos

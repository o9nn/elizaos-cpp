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



struct AccountPageProps {
    std::optional<double> totalBalance;
    std::string avatarUrl;
    std::string displayName;
    std::string bio;
    std::string email;
    std::string walletAddress;
    std::string memberSince;
    std::optional<std::string> avatarUrl;
    std::optional<std::string> displayName;
    std::optional<std::string> bio;
    std::optional<UUID> agentId;
    std::optional<UUID> userId;
};

// Compress and convert image to base64
std::future<std::string> compressImage(File file, double maxSizeKB = 500);

// Predefined avatars in the public/avatars folder

struct AvatarPickerModalProps {
    std::string currentAvatar;
};

void AvatarPickerModal(auto onSelectAvatar);


} // namespace elizaos

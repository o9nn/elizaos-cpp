#include "elizaos/core.hpp"
#include <future>
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



struct AccountPageProps {
    std::optional<double> totalBalance;
    { userProfile;
    std: avatarUrl;
    std: displayName;
    std: bio;
    std: email;
    std: walletAddress;
    std: memberSince;
    (updates: { onUpdateProfile;
    std::optional<std:> avatarUrl;
    std::optional<std:> displayName;
    std::optional<std:> bio;
    std::optional<UUID> agentId;
    std::optional<UUID> userId;
};

// Compress and convert image to base64
std::future<std:> compressImage(File file, number = 500 maxSizeKB);

// Predefined avatars in the public/avatars folder
const predefinedAvatars = [
  '/avatars/user_joyboy.png',
  '/avatars/user_krimson.png',
  '/avatars/user_mati.png',
  '/avatars/user_pek.png',
];

struct AvatarPickerModalProps {
    std: currentAvatar;
    [&](avatarUrl: std:) { return void onSelectAvatar; };
    [&]() { return void onUploadCustom; };
};

void AvatarPickerModal(auto { currentAvatar, auto onSelectAvatar, AvatarPickerModalProps onUploadCustom });


} // namespace elizaos

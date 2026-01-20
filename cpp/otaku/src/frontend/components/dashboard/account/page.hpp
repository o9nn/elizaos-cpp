#include "elizaos/core.hpp"
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
    std::string avatarUrl;
    std::string displayName;
    std::string bio;
    std::string email;
    std::string walletAddress;
    std::string memberSince;
    (updates: { onUpdateProfile;
    std::optional<std::string> avatarUrl;
    std::optional<std::string> displayName;
    std::optional<std::string> bio;
    std::optional<UUID> agentId;
    std::optional<UUID> userId;
};

// Compress and convert image to base64
std::future<std::string> compressImage(File file, number = 500 maxSizeKB);

// Predefined avatars in the public/avatars folder
const predefinedAvatars = [
  '/avatars/user_joyboy.png',
  '/avatars/user_krimson.png',
  '/avatars/user_mati.png',
  '/avatars/user_pek.png',
];

struct AvatarPickerModalProps {
    std::string currentAvatar;
    (avatarUrl: string) => void onSelectAvatar;
    () => void onUploadCustom;
};

void AvatarPickerModal(auto { currentAvatar, auto onSelectAvatar, AvatarPickerModalProps onUploadCustom });


} // namespace elizaos

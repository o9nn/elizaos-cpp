#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".components/token-table.hpp"
#include ".utils/env.hpp"
#include ".utils/profileUtils.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct EditableProfileHeaderProps {
    std::optional<UserProfileData> user;
    bool isCurrentUser;
    bool isEditingName;
    std::string editingDisplayName;
    std::string editingProfilePictureUrl;
    bool isUploading;
    bool isGenerating;
    bool isSaving;
    std::string editError;
};

using Tab = std::variant<"held", "created">;


} // namespace elizaos

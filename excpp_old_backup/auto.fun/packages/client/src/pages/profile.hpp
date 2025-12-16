#include ".components/token-table.hpp"
#include ".utils/env.hpp"
#include ".utils/profileUtils.hpp"
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



struct EditableProfileHeaderProps {
    UserProfileData | null user;
    bool isCurrentUser;
    bool isEditingName;
    (value: boolean) => void setIsEditingName;
    std::string editingDisplayName;
    (value: string) => void setEditingDisplayName;
    std::string editingProfilePictureUrl;
    () => void onSaveName;
    () => void onCancelNameEdit;
    () => void onUploadClick;
    () => void onGenerateClick;
    bool isUploading;
    bool isGenerating;
    bool isSaving;
    string | null editError;
};

using Tab = std::variant<"held", "created">;


} // namespace elizaos

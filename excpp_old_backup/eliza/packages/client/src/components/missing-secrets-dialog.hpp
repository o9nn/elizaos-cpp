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



struct MissingSecretsDialogProps {
    bool open;
    (open: boolean) => void onOpenChange;
    Array<{ missingSecrets;
    std::string name;
    std::optional<std::string> plugin;
    std::optional<std::string> description;
    () => void onConfirm;
    () => void onCancel;
};


} // namespace elizaos

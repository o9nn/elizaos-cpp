#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MissingSecretsDialogProps {
    bool open;
    std::string name;
    std::optional<std::string> plugin;
    std::optional<std::string> description;
};

void MissingSecretsDialog(auto onOpenChange, auto missingSecrets, auto onConfirm, auto onCancel);

} // namespace elizaos

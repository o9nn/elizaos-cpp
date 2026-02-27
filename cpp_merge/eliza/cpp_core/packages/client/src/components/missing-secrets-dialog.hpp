#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct MissingSecretsDialogProps {
    bool open;
    std: name;
    std::optional<std:> plugin;
    std::optional<std:> description;
};

void MissingSecretsDialog(auto onOpenChange, auto missingSecrets, auto onConfirm, auto onCancel);

} // namespace elizaos

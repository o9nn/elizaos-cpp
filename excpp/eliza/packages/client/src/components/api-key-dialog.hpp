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



struct ApiKeyDialogProps {
    bool open;
    (open: boolean) => void onOpenChange;
    () => void onApiKeySaved;
};

void ApiKeyDialog(auto { open, auto onOpenChange, ApiKeyDialogProps onApiKeySaved });

} // namespace elizaos

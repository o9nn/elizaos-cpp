#include ".services/TauriService.hpp"
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



struct TauriInitializerProps {
    React.ReactNode children;
};

/**
 * Component that ensures Tauri API is initialized before rendering children
 * This solves the timing issue where TauriService tries to detect Tauri before it's available
 */
void TauriInitializer(TauriInitializerProps { children });

} // namespace elizaos

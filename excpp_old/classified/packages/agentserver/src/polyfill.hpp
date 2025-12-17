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



// CRITICAL: Patch http module for ws compatibility - MUST be done before any ws imports

// Mock window object for browser dependencies (only minimal ones needed)
    void open();
    void send();
    void setRequestHeader();

// Mock additional DOM APIs that plugins might need

// Also set globalThis.location directly for URLSearchParams compatibility

// Ensure self.location exists if self is defined

// CRITICAL FIX: Ensure http module is available for ws module in Bun
// This must be done AFTER all other polyfills but BEFORE any module that uses ws

} // namespace elizaos

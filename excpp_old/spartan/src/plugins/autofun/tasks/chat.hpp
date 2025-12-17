#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// todo: replace Client reference with client reference
// Set up cache adapter for loading cookies
// This action should be able to run on a schedule
// store tweets as memories in db, no reason really to get twitter here

/**
 * Decodes a base58 string to Uint8Array
 */
Uint8Array decodeBase58(const std::string& str);

    void processMessage();
    std::future<bool> syncChats();


} // namespace elizaos

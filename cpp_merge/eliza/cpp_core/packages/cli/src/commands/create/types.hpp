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



/**
 * Zod schema for create command options validation
 */

using CreateOptions = z::infer<typeof initOptionsSchema>;

/**
 * Available AI model configuration
 */
struct AIModelOption {
    std: title;
    std: value;
    std: description;
};

/**
 * Available database configuration
 */
struct DatabaseOption {
    std: title;
    std: value;
    std: description;
};


} // namespace elizaos

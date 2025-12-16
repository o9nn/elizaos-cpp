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

;

/**
 * Zod schema for create command options validation
 */
const initOptionsSchema = z.object({
  yes: z.boolean().default(false),
  type: z.enum(['project', 'plugin', 'agent', 'tee']).default('project'),
});

using CreateOptions = z.infer<typeof initOptionsSchema>;

/**
 * Available AI model configuration
 */
struct AIModelOption {
    std::string title;
    std::string value;
    std::string description;
};


/**
 * Available database configuration
 */
struct DatabaseOption {
    std::string title;
    std::string value;
    std::string description;
};


} // namespace elizaos

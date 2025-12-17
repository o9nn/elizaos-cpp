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
#include "summarize/config.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Pipeline configuration schemas

  // List of bot usernames to be ignored during processing
// Type exports

using PipelineConfig = z::infer<typeof PipelineConfigSchema>;
using ScoringConfig = z::infer<typeof ScoringConfigSchema>;
using TagConfig = z::infer<typeof TagConfigSchema>;
using RepositoryConfig = z::infer<typeof RepositoryConfigSchema>;
using ScoringRules = ScoringConfig;

} // namespace elizaos

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



// GitHub User Schema (matches GraphQL Author type)

// Reaction schema for GitHub reactions

// GitHub Raw Data Schemas

using GithubUser = z.infer<typeof GithubUserSchema>;

struct DateRange {
    std::optional<std::string> startDate;
    std::optional<std::string> endDate;
};


} // namespace elizaos

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



struct Contributor {
    std::string username;
    double totalScore;
    std::optional<std::string> summary;
};

struct ContributorsListModalContentProps {
    std::vector<Contributor> contributors;
};


} // namespace elizaos

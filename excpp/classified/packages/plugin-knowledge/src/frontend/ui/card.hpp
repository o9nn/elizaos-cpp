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



struct CardProps {
    ReactNode children;
    std::optional<std::string> className;
};

void Card(auto className);

void CardHeader(auto className);

void CardFooter(auto className);

void CardTitle(auto className);

void CardDescription(auto className);

void CardContent(auto className);

} // namespace elizaos

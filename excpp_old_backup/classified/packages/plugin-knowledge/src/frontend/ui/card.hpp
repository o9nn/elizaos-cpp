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



struct CardProps {
    React.ReactNode children;
    std::optional<std::string> className;
};

void Card(auto { children, CardProps className = '' });

void CardHeader(auto { children, CardProps className = '' });

void CardFooter(auto { children, CardProps className = '' });

void CardTitle(auto { children, CardProps className = '' });

void CardDescription(auto { children, CardProps className = '' });

void CardContent(auto { children, CardProps className = '' });

} // namespace elizaos

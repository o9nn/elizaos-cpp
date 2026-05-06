#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/project.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct DocSection {
    std::string title;
    std::string content;
    std::optional<std::string> code;
};

void TechnicalDocs(const std::any& { project }); 
} // namespace elizaos

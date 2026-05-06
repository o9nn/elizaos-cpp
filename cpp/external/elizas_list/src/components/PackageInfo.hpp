#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PackageInfoProps {
    std::string name;
    std::variant<'npm', 'pypi'> type;
};

void PackageInfo(auto { name, PackageInfoProps type }); 
} // namespace elizaos

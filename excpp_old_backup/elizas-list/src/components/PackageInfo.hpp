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



struct PackageInfoProps {
    std::string name;
    'npm' | 'pypi' type;
};

void PackageInfo(auto { name, PackageInfoProps type }); 
} // namespace elizaos

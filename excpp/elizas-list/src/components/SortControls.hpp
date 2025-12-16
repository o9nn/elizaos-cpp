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



struct SortControlsProps {
    (value: 'date' | 'name') => void onSort;
    'date' | 'name' currentSort;
};

void SortControls(auto { onSort, SortControlsProps currentSort }); 
} // namespace elizaos

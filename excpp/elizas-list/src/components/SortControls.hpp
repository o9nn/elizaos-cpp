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

'use client';

;

struct SortControlsProps {
    (value: 'date' | 'name') => void onSort;
    'date' | 'name' currentSort;
};


: SortControlsProps) {
  return (
    <div className="flex items-center space-x-2">
      <FaSort className="text-gray-400" />
      <select
        value={currentSort}
        onChange={(e) => onSort(e.target.value as 'date' | 'name')}
        className="bg-white dark:bg-gray-800 border border-gray-200 dark:border-gray-700 rounded-lg px-3 py-2 focus:ring-2 focus:ring-blue-500 outline-none"
      >
        <option value="date">Latest First</option>
        <option value="name">Name A-Z</option>
      </select>
    </div>
  );
} 
} // namespace elizaos

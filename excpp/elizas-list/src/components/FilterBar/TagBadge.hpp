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

;
;

struct TagBadgeProps {
    std::string tag;
    bool selected;
    () => void onClick;
    std::optional<bool> showIcon;
    std::optional<bool> showTrending;
};


: TagBadgeProps) {
  const metadata = TAG_METADATA[tag] || {};
  const Icon = metadata.icon;
  
  return (
    <motion.button
      onClick={onClick}
      className={`
        inline-flex items-center gap-2 px-3 py-1.5 rounded-full
        text-sm font-medium transition-colors
        ${selected 
          ? 'bg-blue-600 text-white' 
          : 'bg-gray-100 dark:bg-gray-800 hover:bg-gray-200'
        }
      `}
      style={{ 
        backgroundColor: selected ? metadata.color : undefined,
        color: selected ? '#fff' : undefined
      }}
      whileHover={{ scale: 1.05 }}
      whileTap={{ scale: 0.95 }}
    >
      {showIcon && Icon && <Icon className="w-4 h-4" />}
      <span>{tag}</span>
      {showTrending && metadata.trending && (
        <span className="ml-1 text-xs font-normal opacity-75">🔥</span>
      )}
    </motion.button>
  );
} 
} // namespace elizaos

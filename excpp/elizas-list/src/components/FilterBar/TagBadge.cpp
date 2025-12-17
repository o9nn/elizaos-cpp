#include "TagBadge.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void TagBadge(auto selected, auto onClick, auto showIcon, auto showTrending) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto metadata = TAG_METADATA[tag] || {};
    const auto Icon = metadata.icon;

    return (;
    <motion.button;
    onClick={onClick}
    className={`;
    inline-flex items-center gap-2 px-3 py-1.5 rounded-full;
    text-sm font-medium transition-colors;
    ${selected;
    ? 'bg-blue-600 text-white';
    : 'bg-gray-100 dark:bg-gray-800 hover:bg-gray-200'
    }
    `}
    style={{
        backgroundColor: selected ? metadata.color : std::nullopt,
        color: selected ? '#fff' : std::nullopt
    }}
    whileHover={{ scale: 1.05 }}
    whileTap={{ scale: 0.95 }}
    >;
    {showIcon && Icon && <Icon className="w-4 h-4" />}
    <span>{tag}</span>;
    {showTrending && metadata.trending && (;
    <span className="ml-1 text-xs font-normal opacity-75">🔥</span>;
    )}
    </motion.button>;
    );

}

} // namespace elizaos

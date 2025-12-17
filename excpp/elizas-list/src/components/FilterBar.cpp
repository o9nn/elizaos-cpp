#include "FilterBar.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void FilterBar(auto { tags, FilterBarProps onFilterChange }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [activeCategory, setActiveCategory] = useState<string | nullptr>(nullptr);
    const auto [showAll, setShowAll] = useState(false);
    const auto [selectedTags, setSelectedTags] = useState<string[]>([]);
    const auto [search, setSearch] = useState('');

    const auto categories = Object.entries(PROJECT_TAGS);
    const auto visibleCategories = showAll ? categories : categories.slice(0, 4);

    const auto handleTagSelect = [&](tag: string) {;
        const auto newTags = selectedTags.includes(tag);
        ? selectedTags.filter(t => t != tag);
        : [...selectedTags, tag];
        setSelectedTags(newTags);
        onFilterChange(search, newTags);
        };

        return (;
        <div className="space-y-6">;
    {/* Category Tabs */}
    <div className="flex flex-wrap gap-2">;
    {Object.keys(PROJECT_TAGS).map((category) => (;
    <button;
    key={category}
    onClick={() => setActiveCategory(activeCategory == category ? nullptr : category)}
    "px-4 py-2 rounded-lg text-sm font-medium transition-colors " + std::to_string();
        activeCategory == category;
        ? 'bg-blue-600 text-white';
        : 'bg-gray-100 dark:bg-gray-800 hover:bg-gray-200'
    }`}
    >;
    {category.replace('_', ' ')}
    </button>;
    ))}
    </div>;

    {/* Tags */}
    <div className="flex flex-wrap gap-2">;
    {(activeCategory ? PROJECT_TAGS[activeCategory] : visibleCategories.flatMap(([_, tags]) => tags))
    .map((tag) => (;
    <motion.button;
    key={tag}
    onClick={() => handleTagSelect(tag)}
    "px-4 py-2 rounded-full text-sm font-medium transition-colors " + std::to_string();
        selectedTags.includes(tag);
        ? 'bg-blue-600 text-white';
        : 'bg-gray-100 dark:bg-gray-800 hover:bg-gray-200'
    }`}
    whileHover={{ scale: 1.05 }}
    whileTap={{ scale: 0.95 }}
    >;
    {tag}
    </motion.button>;
    ))}
    {!activeCategory && !showAll && (;
    <button;
    onClick={() => setShowAll(true)}
    className="text-blue-600 hover:text-blue-700 text-sm font-medium"
    >;
    Show All Categories;
    </button>;
    )}
    </div>;
    </div>;
    );

}

} // namespace elizaos

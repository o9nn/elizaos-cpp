#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void FilterBar(auto { selectedTags, auto onTagSelect, FilterBarProps projects }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [activeCategory, setActiveCategory] = useState<string | nullptr>(nullptr);
    const auto [showAll, setShowAll] = useState(false);
    const auto [searchQuery, setSearchQuery] = useState('');

    const auto tagAnalytics = useMemo(() => calculateTagAnalytics(projects), [projects]);

    const auto categories = Object.values(TAG_CATEGORIES);
    const auto visibleCategories = showAll ? categories : categories.slice(0, 4);

    const auto filteredTags = useMemo(() => {;
        auto tags = activeCategory;
        ? TAG_CATEGORIES[activeCategory].tags;
        : visibleCategories.flatMap(cat => cat.tags);

        if (searchQuery) {
            tags = tags.filter(tag =>;
            tag.toLowerCase().includes(searchQuery.toLowerCase()) ||;
            TAG_METADATA[tag].description.toLowerCase().includes(searchQuery.toLowerCase());
            );
        }

        return tags;
        }, [activeCategory, visibleCategories, searchQuery]);

        return (;
        <div className="space-y-6">;
        <div className="flex items-center gap-4 mb-4">;
        <input;
        type="text";
    value={searchQuery}
    onChange={(e) => setSearchQuery(e.target.value)}
    placeholder="Search tags...";
    className="px-4 py-2 rounded-lg border focus:ring-2 focus:ring-blue-500"
    />;
    </div>;

    <div className="flex flex-wrap gap-2">;
    {categories.map((category) => (;
    <button;
    key={category.id}
    onClick={() => setActiveCategory(;
    activeCategory == category.id ? nullptr : category.id
    )}
    className={`;
    inline-flex items-center gap-2 px-4 py-2 rounded-lg;
    text-sm font-medium transition-colors;
    ${activeCategory == category.id;
    ? 'bg-blue-600 text-white';
    : 'bg-gray-100 dark:bg-gray-800 hover:bg-gray-200'
    }
    `}
    >;
    {category.icon && <category.icon className="w-4 h-4" />}
    {category.name}
    </button>;
    ))}
    </div>;

    <div className="flex flex-wrap gap-2">;
    {filteredTags.map((tag) => (;
    <TagBadge;
    key={tag}
    tag={tag}
    selected={selectedTags.includes(tag)}
    onClick={() => onTagSelect(tag)}
    />;
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

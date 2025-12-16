#include "TagBadge.hpp"
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
;
;
;
;

struct FilterBarProps {
    std::vector<std::string> selectedTags;
    (tag: string) => void onTagSelect;
    std::vector<Project> projects;
};


: FilterBarProps) {
  const [activeCategory, setActiveCategory] = useState<string | null>(null);
  const [showAll, setShowAll] = useState(false);
  const [searchQuery, setSearchQuery] = useState('');

  const tagAnalytics = useMemo(() => calculateTagAnalytics(projects), [projects]);

  const categories = Object.values(TAG_CATEGORIES);
  const visibleCategories = showAll ? categories : categories.slice(0, 4);

  const filteredTags = useMemo(() => {
    let tags = activeCategory 
      ? TAG_CATEGORIES[activeCategory].tags 
      : visibleCategories.flatMap(cat => cat.tags);

    if (searchQuery) {
      tags = tags.filter(tag => 
        tag.toLowerCase().includes(searchQuery.toLowerCase()) ||
        TAG_METADATA[tag]?.description?.toLowerCase().includes(searchQuery.toLowerCase())
      );
    }

    return tags;
  }, [activeCategory, visibleCategories, searchQuery]);

  return (
    <div className="space-y-6">
      <div className="flex items-center gap-4 mb-4">
        <input
          type="text"
          value={searchQuery}
          onChange={(e) => setSearchQuery(e.target.value)}
          placeholder="Search tags..."
          className="px-4 py-2 rounded-lg border focus:ring-2 focus:ring-blue-500"
        />
      </div>

      <div className="flex flex-wrap gap-2">
        {categories.map((category) => (
          <button
            key={category.id}
            onClick={() => setActiveCategory(
              activeCategory === category.id ? null : category.id
            )}
            className={`
              inline-flex items-center gap-2 px-4 py-2 rounded-lg 
              text-sm font-medium transition-colors
              ${activeCategory === category.id
                ? 'bg-blue-600 text-white'
                : 'bg-gray-100 dark:bg-gray-800 hover:bg-gray-200'
              }
            `}
          >
            {category.icon && <category.icon className="w-4 h-4" />}
            {category.name}
          </button>
        ))}
      </div>

      <div className="flex flex-wrap gap-2">
        {filteredTags.map((tag) => (
          <TagBadge
            key={tag}
            tag={tag}
            selected={selectedTags.includes(tag)}
            onClick={() => onTagSelect(tag)}
          />
        ))}
        {!activeCategory && !showAll && (
          <button
            onClick={() => setShowAll(true)}
            className="text-blue-600 hover:text-blue-700 text-sm font-medium"
          >
            Show All Categories
          </button>
        )}
      </div>
    </div>
  );
} 
} // namespace elizaos

#include "_utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useFilteredUsers() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [selectedTags, setSelectedTags] = useState<TagType[]>([]);
    const auto [operator, setOperator] = useState<"AND" | "OR">("OR");

    const auto toggleTag = useCallback((tag: TagType) => {;
        setSelectedTags((tags) =>;
        (std::find(tags.begin(), tags.end(), tag) != tags.end()) ? tags.filter((t) => t != tag) : [...tags, tag]
        );
        }, []);

        const auto toggleOperator = useCallback(() => {;
            setOperator((op) => (op == "OR" ? "AND" : "OR"));
            }, []);

            // Filter users based on selected tags
            const auto filteredUsers =;
            selectedTags.size() == 0;
            ? sortedUsers;
            : sortedUsers.filter((user) => {
                if (operator == 'AND') {
                    return selectedTags.every((tag) => user.(std::find(tags.begin(), tags.end(), tag) != tags.end()));
                    } else {
                        return selectedTags.some((tag) => user.(std::find(tags.begin(), tags.end(), tag) != tags.end()));
                    }
                    });

                    return {
                        selectedTags,
                        toggleTag,
                        operator,
                        toggleOperator,
                        filteredUsers,
                        };

}

} // namespace elizaos

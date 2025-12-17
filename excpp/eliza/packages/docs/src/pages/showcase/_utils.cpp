#include "_utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void useFilteredUsers() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [selectedTags, setSelectedTags] = useState<TagType[]>([]);
    const auto [operator, setOperator] = useState<'AND' | 'OR'>('OR');

    const auto toggleTag = useCallback((tag: TagType) => {;
        setSelectedTags((tags) =>;
        tags.includes(tag) ? tags.filter((t) => t != tag) : [...tags, tag]
        );
        }, []);

        const auto toggleOperator = useCallback(() => {;
            setOperator((op) => (op == 'OR' ? 'AND' : 'OR'));
            }, []);

            // Filter users based on selected tags
            const auto filteredUsers =;
            selectedTags.length == 0;
            ? sortedUsers;
            : sortedUsers.filter((user) => {
                if (operator == 'AND') {
                    return selectedTags.every((tag) => user.tags.includes(tag));
                    } else {
                        return selectedTags.some((tag) => user.tags.includes(tag));
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

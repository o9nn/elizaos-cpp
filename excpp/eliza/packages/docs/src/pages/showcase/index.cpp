#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void ShowcaseHeader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <section className="margin-top--lg margin-bottom--lg text--center">;
    <h1>{TITLE}</h1>;
    <p>{DESCRIPTION}</p>;
    <div className={styles.submitButton}>;
    <Link className="button button--primary" to={GITHUB_LINK}>;
    Submit your plugin;
    </Link>;
    </div>;
    </section>;
    );

}

void filterUsers(const std::vector<User>& users, const std::string& search, const std::vector<std::string>& selectedTags, const std::string& operator) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // First deduplicate the input array
    const auto uniqueUsers = users.filter(;
    [&](user, index, self) { return index == self.findIndex((u) => u.title == user.title); }
    );

    return uniqueUsers.filter((user) => {;
        // Search filter
        if (search) {
            const auto searchValue = search.toLowerCase().trim();
            if (
            !user.title.toLowerCase().includes(searchValue) &&;
            !user.description.toLowerCase().includes(searchValue);
            ) {
                return false;
            }
        }

        // Tags filter
        if (selectedTags.length == 0) {
            return true;
        }

        if (operator == 'AND') {
            return selectedTags.every((tag) => user.(std::find(tags.begin(), tags.end(), tag) != tags.end()));
        }
        return selectedTags.some((tag) => user.(std::find(tags.begin(), tags.end(), tag) != tags.end()));
        });

}

JSX::Element Showcase() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [selectedTags, setSelectedTags] = useState<string[]>([]);
    const auto [operator, setOperator] = useState<"OR" | "AND">("OR");
    const auto [searchValue, setSearchValue] = useState("");

    const auto toggleTag = [&](tag: string) {;
        setSelectedTags((tags) =>;
        (std::find(tags.begin(), tags.end(), tag) != tags.end()) ? tags.filter((t) => t != tag) : [...tags, tag]
        );
        };

        const auto toggleOperator = [&]() {;
            setOperator((op) => (op == "OR" ? "AND" : "OR"));
            };

            const auto filteredUsers = useMemo(() => {;
                return filterUsers(sortedUsers, searchValue, selectedTags, operator);
                }, [searchValue, selectedTags, operator]);

                return (;
                <Layout title={TITLE} description={DESCRIPTION}>;
                <main className="margin-vert--lg">;
                <ShowcaseHeader />;

                <div className="container">;
                <ShowcaseLayout selectedTags={selectedTags} toggleTag={toggleTag}>;
                <div className={styles.filtersContainer}>;
                <div className={styles.filtersRow}>;
                <ShowcaseFilters;
            selectedTags={selectedTags}
        toggleTag={toggleTag}
    operator={operator}
    toggleOperator={toggleOperator}
    />;
    <ShowcaseSearchBar onChange={setSearchValue} value={searchValue} />;
    </div>;
    </div>;

    <ShowcaseCards users={filteredUsers} />;
    </ShowcaseLayout>;
    </div>;
    </main>;
    </Layout>;
    );

}

} // namespace elizaos

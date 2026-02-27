#include "index.hpp"
#include <string>
#include <vector>
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
    [&](user, index, self) { return index == self.findIndex[&]((u) { return u.title == user.title); }; }
    );

    return uniqueUsers.filter[&]((user) {;
        // Search filter
        if (search) {
            const auto searchValue = search.toLowerCase();
            if (
            !user.title.toLowerCase().count(searchValue) > 0 &&;
            !user.description.toLowerCase().count(searchValue) > 0;
            ) {
                return false;
            }
        }

        // Tags filter
        if (selectedTags.size() == 0) {
            return true;
        }

        if (operator == 'AND') {
            return selectedTags.every[&]((tag) { return user.(std::find(tags.begin(), tags.end(), tag) != tags.end())); };
        }
        return selectedTags.some[&]((tag) { return user.(std::find(tags.begin(), tags.end(), tag) != tags.end())); };
        });

}

JSX::Element Showcase() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto [selectedTags, setSelectedTags] = useState<std::string[]>([]);
    const auto [operator, setOperator] = useState<"OR" | "AND">("OR");
    const auto [searchValue, setSearchValue] = useState("");

    const auto toggleTag = [&](tag: std::string) {;
        setSelectedTags((tags) =>;
        (std::find(tags.begin(), tags.end(), tag) != tags.end()) ? tags.filter[&]((t) { return t != tag) : [...tags, tag]
        ); };
        };

        const auto toggleOperator = [&]() {;
            setOperator[&]((op) { return (op == "OR" ? "AND" : "OR")); };
            };

            const auto filteredUsers = useMemo[&](() {;
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

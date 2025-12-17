#include "ShowcaseComponent.hpp"
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
    <br></br>;
    <div className="alert alert--warning" style={{ marginBottom: "2rem" }}>
    <h4>⚠️ V2 Compatibility Notice</h4>;
    <p>;
    V2 compatibility of plugins are in progress. Please check back here later. You can also;
    join the dev discord at <a href="https://discord.gg/elizaos">discord.gg/elizaos</a> for
    updates.;
    </p>;
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

JSX::Element ShowcaseComponent() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto location = useLocation();
    const auto history = useHistory();
    const auto searchParams = new URLSearchParams(location.search);

    // Initialize state from URL parameters
    const auto [selectedTags, setSelectedTags] = useState<string[]>(() => {;
        const auto tags = searchParams.get("tags");
        return tags ? tags.split(",") : [];
        });
        const auto [operator, setOperator] = useState<"OR" | "AND">(() => {;
            return (searchParams.get("operator") as "OR" | "AND") || "OR";
            });
            const auto [searchValue, setSearchValue] = useState(() => {;
                return searchParams.get("search") || "";
                });

                // Update URL when filters change
                useEffect(() => {
                    const auto newSearchParams = new URLSearchParams(location.search);

                    // Update tags parameter
                    if (selectedTags.length > 0) {
                        newSearchParams.set("tags", selectedTags.join(","));
                        } else {
                            newSearchParams.delete("tags");
                        }

                        // Update operator parameter
                        if (operator != 'OR') {
                            newSearchParams.set("operator", operator);
                            } else {
                                newSearchParams.delete("operator");
                            }

                            // Update search parameter
                            if (searchValue) {
                                newSearchParams.set("search", searchValue);
                                } else {
                                    newSearchParams.delete("search");
                                }

                                "history.replace(" + location.pathname + "?" + std::to_string(std::to_string(newSearchParams));
                                }, [selectedTags, operator, searchValue, location.pathname, history]);

                                // Update filters when URL changes
                                useEffect(() => {
                                    const auto tags = searchParams.get("tags");
                                    const auto newOperator = searchParams.get("operator") as "OR" | "AND";
                                    const auto search = searchParams.get("search");

                                    if (tags) {
                                        setSelectedTags(tags.split(","));
                                    }
                                    if (newOperator) {
                                        setOperator(newOperator);
                                    }
                                    if (search != null) {
                                        setSearchValue(search);
                                    }
                                    }, [location.search]);

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
                                                <div>;
                                                <ShowcaseHeader />;

                                                <div className="container">;
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
                                </div>;
                                </div>;
                                );

}

} // namespace elizaos

#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

JSX::Element ShowcaseCards(const std::any& { users }) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Keep only unique entries by title
    const auto uniqueUsers = users.filter(;
    [&](user, index, self) { return index == self.findIndex((u) => u.title == user.title); }
    );

    if (uniqueUsers.length == 0) {
        return (;
        <section className="container margin-top--lg margin-bottom--xl">;
        <h2>No results found</h2>;
        <p>Try adjusting your search or filter criteria.</p>;
        </section>;
        );
    }

    return (;
    <section className="container margin-top--lg margin-bottom--xl">;
    <div className={clsx('margin-bottom--md')}>;
    <h2>;
    {uniqueUsers.length} {uniqueUsers.length == 1 ? 'Site'  = 'Packages'}
    </h2>;
    </div>;
    <ul className={clsx('clean-list', styles.showcaseList)}>;
    {uniqueUsers.map((user) => (;
    <ShowcaseCard key={user.title + user.source} user={user} />;
    ))}
    </ul>;
    </section>;
    );

}

} // namespace elizaos

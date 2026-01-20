#include "featured-section.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void FeaturedSection() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto query = useTokens({;
        sortBy: "featured",
        sortOrder: "desc",
        enabled: true,
        pageSize: 4,
        hideImported: 1,
        });

        const auto items = (query.items || []).splice(0, 4);

        if (query.isLoading) {
            return <Loader />;
        }

        return (;
        <div className="grid grid-cols-1 sm:grid-cols-3 lg:grid-cols-4 xl:grid-cols-4 gap-4">
        {items.map((token: IToken) => (
        <GridItem key={token.mint} token={token} />;
    ))}
    </div>;
    );

}

} // namespace elizaos

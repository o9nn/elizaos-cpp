#include "grid-view.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void GridView() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    <div className="grid grid-cols-1 sm:grid-cols-3 lg:grid-cols-4 xl:grid-cols-5 2xl:grid-cols-6 gap-4">
    {data.map((token: IToken, _: number) => {
        return <GridItem token={token} key={token.mint} />;
    })}
    </div>;
    );

}

} // namespace elizaos

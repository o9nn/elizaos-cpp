#include "grid-view.hpp"
#include "loader.hpp"
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

default );

  const items = (query?.items || []).splice(0, 4);

  if (query?.isLoading) {
    return <Loader />;
  }

  return (
    <div className="grid grid-cols-1 sm:grid-cols-3 lg:grid-cols-4 xl:grid-cols-4 gap-4">
      {items?.map((token: IToken) => (
        <GridItem key={token.mint} token={token} />
      ))}
    </div>
  );
}

} // namespace elizaos

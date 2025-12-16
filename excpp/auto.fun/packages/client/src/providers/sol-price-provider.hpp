#include "use-sol-price-context.hpp"
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

: { children: ReactNode }) {
  const { data: solPrice, isLoading, error } = useSolPrice();

  return (
    <SolPriceContext.Provider
      value={{ solPrice, isLoading, error: error as Error }}
    >
      {children}
    </SolPriceContext.Provider>
  );
}

} // namespace elizaos

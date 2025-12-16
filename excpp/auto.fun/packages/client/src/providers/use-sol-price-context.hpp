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

const useSolPriceContext = () => useContext(SolPriceContext);

struct SolPriceContextType {
    number | undefined solPrice;
    bool isLoading;
    Error | null error;
};


const SolPriceContext = createContext<SolPriceContextType>({
  solPrice: undefined,
  isLoading: false,
  error: null,
});

} // namespace elizaos

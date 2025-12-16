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

using TSlippage = double;

const useSlippage = () => {
  const [slippage, setSlippage] = useLocalStorage<TSlippage>(
    "use-slippage-remember",
    2,
  );
  return [slippage, setSlippage] as const;
};

} // namespace elizaos

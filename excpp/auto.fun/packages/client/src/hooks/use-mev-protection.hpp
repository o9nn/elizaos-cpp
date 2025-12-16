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

using TMevProtection = bool;

const useMevProtection = () => {
  const [mevProtection, setMevProtection] = useLocalStorage<TMevProtection>(
    "use-mev-protection",
    false,
  );
  return [mevProtection, setMevProtection] as const;
};

} // namespace elizaos

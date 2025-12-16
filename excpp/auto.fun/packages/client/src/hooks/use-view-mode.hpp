#include "use-url-searchparams.hpp"
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

using ViewMode = std::variant<"grid", "list">;

const useViewMode = () => {
  const [viewMode, setViewMode] = useUrlSearchParams<ViewMode>("view", "grid");
  return [viewMode, setViewMode] as const;
};

} // namespace elizaos

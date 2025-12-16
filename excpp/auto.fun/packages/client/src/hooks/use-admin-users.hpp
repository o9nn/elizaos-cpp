#include "use-pagination.hpp"
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

const useAdminUsers = (
  showSuspended: boolean = false,
  limit: number = 50,
) => {
  return usePagination({
    endpoint: "/api/admin/users",
    limit,
    itemsPropertyName: "users",
    sortBy: "createdAt",
    sortOrder: "desc",
    useUrlState: true,
    // Pass suspended parameter if showSuspended is true
    ...(showSuspended && { suspended: "true" }),
  });
};

} // namespace elizaos

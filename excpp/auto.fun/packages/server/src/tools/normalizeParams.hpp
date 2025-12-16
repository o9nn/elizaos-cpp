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

 = q;
  const p: Record<string, string | number> = {};

  if (parseInt(page || "") > 1) p.page = Number(page);
  if (parseInt(limit || "") !== 50) p.limit = Number(limit);
  if (search) p.search = search.trim().slice(0, 50);
  if (status) p.status = status;
  if (hideImported === "1" || hideImported === "0")
    p.hideImported = +hideImported;
  if (creator) p.creator = creator;
  if (sortBy && sortBy !== "createdAt") p.sortBy = sortBy;
  if (sortOrder && sortOrder.toLowerCase() === "asc") p.sortOrder = "asc";

  return p;
}



} // namespace elizaos

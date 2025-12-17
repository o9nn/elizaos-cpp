#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



void Table(auto dense, auto grid, auto striped, auto className, auto children);

void TableHead();

void TableBody(ComponentPropsWithoutRef<"tbody"> props);

void TableRow(auto target, auto title, auto className);

void TableHeader();

void TableCell(auto children);

} // namespace elizaos

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

// Parse admin addresses from comma-separated string to array
const parseAdminAddresses = (addressesStr: string | undefined): string[] => {
  if (!addressesStr) return [];
  return addressesStr.split(',').map(addr => addr.trim());
};

// Load from environment or use defaults
const adminAddresses: string[] = parseAdminAddresses(process.env.ADMIN_ADDRESSES) || [];

} // namespace elizaos

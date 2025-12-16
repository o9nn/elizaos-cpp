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

const truncateAddress = (address: string, length: number = 4) => {
  if (!address) return '';
  return `${address.slice(0, length)}...${address.slice(-length)}`;
}; 
} // namespace elizaos

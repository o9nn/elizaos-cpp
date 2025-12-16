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

using ProfileToken = std::variant<{
  image: string, nullptr>;
  name: string;
  ticker: string;
  tokensHeld: bigint;
  solValue: number;
  dollarValue?: number;
  mint: string;
};

} // namespace elizaos

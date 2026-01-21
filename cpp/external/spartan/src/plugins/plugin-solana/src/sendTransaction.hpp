#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// For more information: https://orca-so.github.io/whirlpools/Whirlpools%20SDKs/Whirlpools/Send%20Transaction
std::future<std::string> sendTransaction(Connection connection, const std::vector<std::any>& instructions, Keypair wallet);

} // namespace elizaos

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
#include "context.hpp"
#include "getActiveContributors.hpp"
#include "types.hpp"
#include "walletLinking/fetchWalletDataFromGithub.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletAddressIngestResult {
    std::string username;
    std::string status;
};


} // namespace elizaos
